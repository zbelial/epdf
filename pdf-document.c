#include <mupdf/fitz.h>
#include <mupdf/pdf.h>

#include <glib-2.0/glib.h>

#include "types.h"

epdf_error_t
pdf_document_open(epdf_document_t* document)
{
    epdf_error_t error = EPDF_ERROR_OK;
    if (document == NULL) {
        error = EPDF_ERROR_INVALID_ARGUMENTS;
        goto error_ret;
    }

    mupdf_document_t* mupdf_document = calloc(1, sizeof(mupdf_document_t));
    if (mupdf_document == NULL) {
        error = EPDF_ERROR_OUT_OF_MEMORY;
        goto error_ret;
    }

    mupdf_document->ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
    if (mupdf_document->ctx == NULL) {
        error = EPDF_ERROR_UNKNOWN;
        goto error_free;
    }

    /* open document */
    const char* path     = epdf_document_get_path(document);
    const char* password = epdf_document_get_password(document);

    fz_try(mupdf_document->ctx){
        fz_register_document_handlers(mupdf_document->ctx);

        mupdf_document->document = fz_open_document(mupdf_document->ctx, path);
    }
    fz_catch(mupdf_document->ctx){
        error = EPDF_ERROR_UNKNOWN;
        return error;
    }

    if (mupdf_document->document == NULL) {
        error = EPDF_ERROR_UNKNOWN;
        goto error_free;
    }

    /* authenticate if password is required and given */
    if (fz_needs_password(mupdf_document->ctx, mupdf_document->document) != 0) {
        if (password == NULL || fz_authenticate_password(mupdf_document->ctx, mupdf_document->document, password) == 0) {
            error = EPDF_ERROR_INVALID_PASSWORD;
            goto error_free;
        }
    }

    epdf_document_set_number_of_pages(document, fz_count_pages(mupdf_document->ctx, mupdf_document->document));
    epdf_document_set_data(document, mupdf_document);

    return EPDF_ERROR_OK;

error_free:

    if (mupdf_document != NULL) {
        if (mupdf_document->document != NULL) {
            fz_drop_document(mupdf_document->ctx, mupdf_document->document);
        }
        if (mupdf_document->ctx != NULL) {
            fz_drop_context(mupdf_document->ctx);
        }

        free(mupdf_document);
    }

    epdf_document_set_data(document, NULL);

error_ret:

    return error;
}

epdf_error_t
pdf_document_free(epdf_document_t* document, void* data)
{
    mupdf_document_t* mupdf_document = data;

    if (document == NULL || mupdf_document == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    fz_drop_document(mupdf_document->ctx, mupdf_document->document);
    fz_drop_context(mupdf_document->ctx);
    free(mupdf_document);
    epdf_document_set_data(document, NULL);

    return EPDF_ERROR_OK;
}

epdf_error_t
pdf_document_save_as(epdf_document_t* document, void* data, const char* path)
{
    mupdf_document_t* mupdf_document = data;

    if (document == NULL || mupdf_document == NULL || path == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    fz_try (mupdf_document->ctx) {
        pdf_save_document(mupdf_document->ctx, (pdf_document*) mupdf_document->document, path, NULL);
    } fz_catch (mupdf_document->ctx) {
        return EPDF_ERROR_UNKNOWN;
    }

    return EPDF_ERROR_OK;
}

