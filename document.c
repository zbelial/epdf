#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <glib.h>
#include <gio/gio.h>
#include <math.h>

#include "document.h"
#include "page.h"

static void
check_set_error(epdf_error_t* error, epdf_error_t code) {
    if (error != NULL) {
        *error = code;
    }
}

static bool
hash_file_sha256(uint8_t* dst, const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }

    GChecksum* checksum = g_checksum_new(G_CHECKSUM_SHA256);
    if (checksum == NULL) {
        fclose(f);
        return false;
    }

    uint8_t buf[BUFSIZ];
    size_t read;
    while ((read = fread(buf, 1, sizeof(buf), f)) != 0) {
        g_checksum_update(checksum, buf, read);
    }

    if (ferror(f) != 0) {
        g_checksum_free(checksum);
        fclose(f);
        return false;
    }

    fclose(f);
    gsize dst_size = 32;
    g_checksum_get_digest(checksum, dst, &dst_size);
    g_checksum_free(checksum);
    return true;
}

epdf_document_t*
epdf_document_open(epdf_t* epdf, const char* path, const char* uri,
                   const char* password, epdf_error_t* error)
{
    if (epdf == NULL || path == NULL) {
        return NULL;
    }

    GFile* file = g_file_new_for_path(path);
    char* real_path = NULL;
    char* content_type = NULL;
    epdf_plugin_t* plugin = NULL;
    epdf_document_t* document = NULL;

    if (file == NULL) {
        check_set_error(error, EPDF_ERROR_UNKNOWN);
        goto error_free;
    }

    real_path = g_file_get_path(file);
    if (real_path == NULL) {
        check_set_error(error, EPDF_ERROR_UNKNOWN);
        goto error_free;
    }

    content_type = epdf_content_type_guess(epdf->content_type_context, real_path, epdf_plugin_manager_get_content_types(epdf->plugins.manager));
    if (content_type == NULL) {
        check_set_error(error, EPDF_ERROR_UNKNOWN);
        goto error_free;
    }

    plugin = epdf_plugin_manager_get_plugin(epdf->plugins.manager, content_type);
    if (plugin == NULL) {
        check_set_error(error, EPDF_ERROR_UNKNOWN);

        g_free((void*)content_type);
        content_type = NULL;

        goto error_free;
    }

    g_free(content_type);
    content_type = NULL;

    document = g_try_malloc0(sizeof(epdf_document_t));
    if (document == NULL) {
        check_set_error(error, EPDF_ERROR_OUT_OF_MEMORY);
        goto error_free;
    }

    document->file_path   = real_path;
    document->uri         = g_strdup(uri);
    if (document->uri == NULL) {
        document->basename    = g_file_get_basename(file);
    } else {
        GFile*gf = g_file_new_for_uri(document->uri);
        document->basename = g_file_get_basename(gf);
        g_object_unref(gf);
    }
    hash_file_sha256(document->hash_sha256, document->file_path);
    document->password    = password;
    document->zoom        = 1.0;
    document->plugin      = plugin;
    document->adjust_mode = EPDF_ADJUST_NONE;
    document->cell_width  = 0.0;
    document->cell_height = 0.0;
    document->view_height = 0;
    document->view_width  = 0;
    document->view_ppi    = 0.0;
    document->device_factors.x = 1.0;
    document->device_factors.y = 1.0;
    document->position_x  = 0.0;
    document->position_y  = 0.0;

    real_path = NULL;
    g_object_unref(file);
    file = NULL;

    /* open document */
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->document_open == NULL) {
        goto error_free;
    }

    epdf_error_t int_error = functions->document_open(document);
    if (int_error != EPDF_ERROR_OK) {
        check_set_error(error, int_error);
        goto error_free;
    }

    /* read all pages */
    document->pages = calloc(document->number_of_pages, sizeof(epdf_page_t*));
    if (document->pages == NULL) {
        check_set_error(error, EPDF_ERROR_OUT_OF_MEMORY);
        goto error_free;
    }

    for (unsigned int page_id = 0; page_id < document->number_of_pages; page_id++) {
        epdf_page_t* page = epdf_page_new(document, page_id, NULL);
        if (page == NULL) {
            check_set_error(error, EPDF_ERROR_OUT_OF_MEMORY);
            goto error_free;
        }

        document->pages[page_id] = page;

        /* cell_width and cell_height is the maximum of all the pages width and height */
        const double width = epdf_page_get_width(page);
        if (document->cell_width < width)
            document->cell_width = width;

        const double height = epdf_page_get_height(page);
        if (document->cell_height < height)
            document->cell_height = height;
    }

    return document;

error_free:

    if (file != NULL) {
        g_object_unref(file);
    }

    g_free(real_path);

    if (document != NULL) {
        epdf_document_free(document);
        document = NULL; /* prevent double-free */
    }

    g_free(document);

    return NULL;
}

epdf_error_t
epdf_document_free(epdf_document_t* document)
{
    if (document == NULL || document->plugin == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    if (document->pages != NULL) {
        /* free pages */
        for (unsigned int page_id = 0; page_id < document->number_of_pages; page_id++) {
            epdf_page_free(document->pages[page_id]);
            document->pages[page_id] = NULL;
        }
        free(document->pages);
    }

    /* free document */
    epdf_error_t error = EPDF_ERROR_OK;
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(document->plugin);
    if (functions->document_free == NULL) {
        error = EPDF_ERROR_NOT_IMPLEMENTED;
    } else {
        error = functions->document_free(document, document->data);
    }

    g_free(document->file_path);
    g_free(document->uri);
    g_free(document->basename);

    g_free(document);

    return error;
}

const char*
epdf_document_get_path(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->file_path;
}

const uint8_t*
epdf_document_get_hash(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->hash_sha256;
}

const char*
epdf_document_get_uri(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->uri;
}

const char*
epdf_document_get_basename(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->basename;
}

const char*
epdf_document_get_password(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->password;
}

epdf_page_t*
epdf_document_get_page(epdf_document_t* document, unsigned int index)
{
    if (document == NULL || document->pages == NULL || (document->number_of_pages <= index)) {
        return NULL;
    }

    return document->pages[index];
}

void*
epdf_document_get_data(epdf_document_t* document)
{
    if (document == NULL) {
        return NULL;
    }

    return document->data;
}

void
epdf_document_set_data(epdf_document_t* document, void* data)
{
    if (document == NULL) {
        return;
    }

    document->data = data;
}

unsigned int
epdf_document_get_number_of_pages(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->number_of_pages;
}

void
epdf_document_set_number_of_pages(epdf_document_t* document, unsigned int number_of_pages)
{
    if (document == NULL) {
        return;
    }

    document->number_of_pages = number_of_pages;
}

unsigned int
epdf_document_get_current_page_number(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->current_page_number;
}

void
epdf_document_set_current_page_number(epdf_document_t* document, unsigned int
                                      current_page)
{
    if (document == NULL) {
        return;
    }

    document->current_page_number = current_page;
}

double
epdf_document_get_position_x(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->position_x;
}

double
epdf_document_get_position_y(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->position_y;
}

void
epdf_document_set_position_x(epdf_document_t* document, double position_x)
{
    if (document == NULL) {
        return;
    }

    document->position_x = position_x;
}

void
epdf_document_set_position_y(epdf_document_t* document, double position_y)
{
    if (document == NULL) {
        return;
    }

    document->position_y = position_y;
}

double
epdf_document_get_zoom(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->zoom;
}

void
epdf_document_set_zoom(epdf_document_t* document, double zoom)
{
    if (document == NULL) {
        return;
    }

    document->zoom = zoom;
}

double
epdf_document_get_scale(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    double ppi = document->view_ppi;
    if (ppi < DBL_EPSILON) {
        /* No PPI information -> use a typical value */
        ppi = 100;
    }

    /* scale = pixels per point, and there are 72 points in one inch */
    return document->zoom * ppi / 72.0;
}

unsigned int
epdf_document_get_rotation(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->rotate;
}

void
epdf_document_set_rotation(epdf_document_t* document, unsigned int rotation)
{
    if (document == NULL) {
        return;
    }

    rotation = rotation % 360;
    if (rotation == 0 || rotation > 270) {
        document->rotate = 0;
    } else if (rotation <= 90) {
        document->rotate = 90;
    } else if (rotation <= 180) {
        document->rotate = 180;
    } else {
        document->rotate = 270;
    }
}

epdf_adjust_mode_t
epdf_document_get_adjust_mode(epdf_document_t* document)
{
    if (document == NULL) {
        return EPDF_ADJUST_NONE;
    }

    return document->adjust_mode;
}

void
epdf_document_set_adjust_mode(epdf_document_t* document, epdf_adjust_mode_t mode)
{
    if (document == NULL) {
        return;
    }

    document->adjust_mode = mode;
}

int
epdf_document_get_page_offset(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }

    return document->page_offset;
}

void
epdf_document_set_page_offset(epdf_document_t* document, unsigned int page_offset)
{
    if (document == NULL) {
        return;
    }

    document->page_offset = page_offset;
}

void
epdf_document_set_viewport_width(epdf_document_t* document, unsigned int width)
{
    if (document == NULL) {
        return;
    }
    document->view_width = width;
}

void
epdf_document_set_viewport_height(epdf_document_t* document, unsigned int height)
{
    if (document == NULL) {
        return;
    }
    document->view_height = height;
}

void
epdf_document_set_viewport_ppi(epdf_document_t* document, double ppi)
{
    if (document == NULL) {
        return;
    }
    document->view_ppi = ppi;
}

void
epdf_document_get_viewport_size(epdf_document_t* document,
                                unsigned int *height, unsigned int* width)
{
    g_return_if_fail(document != NULL && height != NULL && width != NULL);
    *height = document->view_height;
    *width = document->view_width;
}

double
epdf_document_get_viewport_ppi(epdf_document_t* document)
{
    if (document == NULL) {
        return 0.0;
    }
    return document->view_ppi;
}

void
epdf_document_set_device_factors(epdf_document_t* document,
                                 double x_factor, double y_factor)
{
    if (document == NULL) {
        return;
    }
    if (fabs(x_factor) < DBL_EPSILON || fabs(y_factor) < DBL_EPSILON) {
        x_factor, y_factor);
    return;
}

document->device_factors.x = x_factor;
document->device_factors.y = y_factor;
}

epdf_device_factors_t
epdf_document_get_device_factors(epdf_document_t* document)
{
    if (document == NULL) {
        /* The function is guaranteed to not return zero values */
        return (epdf_device_factors_t){1.0, 1.0};
    }

    return document->device_factors;
}

void
epdf_document_get_cell_size(epdf_document_t* document,
                            unsigned int* height, unsigned int* width)
{
    g_return_if_fail(document != NULL && height != NULL && width != NULL);

    page_calc_height_width(document, document->cell_height, document->cell_width,
                           height, width, true);
}

void
epdf_document_get_document_size(epdf_document_t* document,
                                unsigned int* height, unsigned int* width)
{
    g_return_if_fail(document != NULL && height != NULL && width != NULL);

    const unsigned int npag = epdf_document_get_number_of_pages(document);
    const unsigned int ncol = epdf_document_get_pages_per_row(document);

    if (npag == 0 || ncol == 0) {
        return;
    }

    const unsigned int c0    = epdf_document_get_first_page_column(document);
    const unsigned int nrow  = (npag + c0 - 1 + ncol - 1) / ncol;  /* number of rows */
    const unsigned int pad   = epdf_document_get_page_padding(document);

    unsigned int cell_height = 0;
    unsigned int cell_width  = 0;
    epdf_document_get_cell_size(document, &cell_height, &cell_width);

    *width  = ncol * cell_width + (ncol - 1) * pad;
    *height = nrow * cell_height + (nrow - 1) * pad;
}

void
epdf_document_set_page_layout(epdf_document_t* document, unsigned int page_padding,
                              unsigned int pages_per_row, unsigned int first_page_column)
{
    g_return_if_fail(document != NULL);

    document->page_padding = page_padding;
    document->pages_per_row = pages_per_row;

    if (first_page_column < 1) {
        first_page_column = 1;
    } else if (first_page_column > pages_per_row) {
        first_page_column = ((first_page_column - 1) % pages_per_row) + 1;
    }

    document->first_page_column = first_page_column;
}

unsigned int
epdf_document_get_page_padding(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }
    return document->page_padding;
}

unsigned int
epdf_document_get_pages_per_row(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }
    return document->pages_per_row;
}

unsigned int
epdf_document_get_first_page_column(epdf_document_t* document)
{
    if (document == NULL) {
        return 0;
    }
    return document->first_page_column;
}

epdf_error_t
epdf_document_save_as(epdf_document_t* document, const char* path)
{
    if (document == NULL || document->plugin == NULL || path == NULL) {
        return EPDF_ERROR_UNKNOWN;
    }

    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(document->plugin);
    if (functions->document_save_as == NULL) {
        return EPDF_ERROR_NOT_IMPLEMENTED;
    }

    return functions->document_save_as(document, document->data, path);
}

epdf_error_t
epdf_document_attachment_save(epdf_document_t* document, const char* attachment, const char* file)
{
    if (document == NULL || document->plugin == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(document->plugin);
    if (functions->document_attachment_save == NULL) {
        return EPDF_ERROR_NOT_IMPLEMENTED;
    }

    return functions->document_attachment_save(document, document->data, attachment, file);
}

