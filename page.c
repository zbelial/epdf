#include <glib/gi18n.h>

#include "document.h"
#include "page.h"
#include "types.h"

epdf_page_t*
epdf_page_new(epdf_document_t* document, unsigned int index, epdf_error_t* error)
{
    if (document == NULL) {
        if (error != NULL) {
            *error = EPDF_ERROR_INVALID_ARGUMENTS;
        }
        goto error_ret;
    }

    /* init page */
    epdf_page_t* page = g_try_malloc0(sizeof(epdf_page_t));
    if (page == NULL) {
        if (error != NULL) {
            *error = EPDF_ERROR_OUT_OF_MEMORY;
        }
        goto error_ret;
    }

    page->index    = index;
    page->visible  = false;
    page->document = document;

    /* init plugin */
    epdf_plugin_t* plugin = epdf_document_get_plugin(document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);

    if (functions->page_init == NULL) {
        if (error != NULL) {
            *error = EPDF_ERROR_NOT_IMPLEMENTED;
        }
        goto error_free;
    }

    epdf_error_t ret = functions->page_init(page);
    if (ret != EPDF_ERROR_OK) {
        if (error != NULL) {
            *error = ret;
        }
        goto error_free;
    }

    return page;

error_free:

    if (page != NULL) {
        epdf_page_free(page);
    }

error_ret:

    return NULL;
}

epdf_error_t
epdf_page_free(epdf_page_t* page)
{
    if (page == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    if (page->document == NULL) {
        g_free(page);
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    epdf_plugin_t* plugin = epdf_document_get_plugin(page->document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->page_clear == NULL) {
        return EPDF_ERROR_NOT_IMPLEMENTED;
    }

    epdf_error_t error = functions->page_clear(page, page->data);

    g_free(page);

    return error;
}

epdf_document_t*
epdf_page_get_document(epdf_page_t* page)
{
    if (page == NULL) {
        return NULL;
    }

    return page->document;
}

unsigned int
epdf_page_get_index(epdf_page_t* page)
{
    if (page == NULL) {
        return 0;
    }

    return page->index;
}

double
epdf_page_get_width(epdf_page_t* page)
{
    if (page == NULL) {
        return -1;
    }

    return page->width;
}

void
epdf_page_set_width(epdf_page_t* page, double width)
{
    if (page == NULL) {
        return;
    }

    page->width = width;
}

double
epdf_page_get_height(epdf_page_t* page)
{
    if (page == NULL) {
        return -1;
    }

    return page->height;
}

void
epdf_page_set_height(epdf_page_t* page, double height)
{
    if (page == NULL) {
        return;
    }

    page->height = height;
}

bool
epdf_page_get_visibility(epdf_page_t* page)
{
    if (page == NULL) {
        return false;
    }

    return page->visible;
}

void
epdf_page_set_visibility(epdf_page_t* page, bool visibility)
{
    if (page == NULL) {
        return;
    }

    page->visible = visibility;
}

void*
epdf_page_get_data(epdf_page_t* page)
{
    if (page == NULL) {
        return NULL;
    }

    return page->data;
}

void
epdf_page_set_data(epdf_page_t* page, void* data)
{
    if (page == NULL) {
        return;
    }

    page->data = data;
}

cairo_surface_t*
epdf_page_image_get_cairo(epdf_page_t* page, epdf_image_t* image, epdf_error_t* error)
{
    if (page == NULL || page->document == NULL  || image == NULL) {
        if (error != NULL) {
            *error = EPDF_ERROR_INVALID_ARGUMENTS;
        }
        return NULL;
    }

    epdf_plugin_t* plugin = epdf_document_get_plugin(page->document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->page_image_get_cairo == NULL) {
        if (error != NULL) {
            *error = EPDF_ERROR_NOT_IMPLEMENTED;
        }
        return NULL;
    }

    return functions->page_image_get_cairo(page, page->data, image, error);
}

char*
epdf_page_get_text(epdf_page_t* page, epdf_rectangle_t rectangle, epdf_error_t* error)
{
    if (page == NULL || page->document == NULL ) {
        if (error) {
            *error = EPDF_ERROR_INVALID_ARGUMENTS;
        }
        return NULL;
    }

    epdf_plugin_t* plugin = epdf_document_get_plugin(page->document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->page_get_text == NULL) {
        if (error) {
            *error = EPDF_ERROR_NOT_IMPLEMENTED;
        }
        return NULL;
    }

    return functions->page_get_text(page, page->data, rectangle, error);
}

epdf_error_t
epdf_page_render(epdf_page_t* page, cairo_t* cairo, bool printing)
{
    if (page == NULL  || page->document == NULL || cairo == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    epdf_plugin_t* plugin = epdf_document_get_plugin(page->document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->page_render_cairo == NULL) {
        return EPDF_ERROR_NOT_IMPLEMENTED;
    }

    return functions->page_render_cairo(page, page->data, cairo, printing);
}

char*
epdf_page_get_label(epdf_page_t* page, epdf_error_t* error)
{
    if (page == NULL || page->document == NULL) {
        if (error) {
            *error = EPDF_ERROR_INVALID_ARGUMENTS;
        }
        return NULL;
    }

    epdf_plugin_t* plugin = epdf_document_get_plugin(page->document);
    const epdf_plugin_functions_t* functions = epdf_plugin_get_functions(plugin);
    if (functions->page_get_label == NULL) {
        if (error) {
            *error = EPDF_ERROR_NOT_IMPLEMENTED;
        }
        return NULL;
    }

    char* ret = NULL;
    epdf_error_t e = functions->page_get_label(page, page->data, &ret);
    if (e != EPDF_ERROR_OK) {
        if (error) {
            *error = e;
        }
        return NULL;
    }

    return ret;
}
