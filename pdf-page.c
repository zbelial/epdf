#include "types.h"

epdf_error_t
pdf_page_init(epdf_page_t* page)
{
    if (page == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    epdf_document_t* document        = epdf_page_get_document(page);
    mupdf_document_t* mupdf_document = epdf_document_get_data(document);
    mupdf_page_t* mupdf_page         = calloc(1, sizeof(mupdf_page_t));
    unsigned int index               = epdf_page_get_index(page);

    if (mupdf_page == NULL) {
        return  EPDF_ERROR_OUT_OF_MEMORY;
    }

    mupdf_page->ctx = mupdf_document->ctx;
    if (mupdf_page->ctx == NULL) {
        goto error_free;
    }

    /* load page */
    fz_try (mupdf_page->ctx) {
        mupdf_page->page = fz_load_page(mupdf_document->ctx, mupdf_document->document, index);
    } fz_catch (mupdf_page->ctx) {
        goto error_free;
    }

    mupdf_page->bbox = fz_bound_page(mupdf_document->ctx, (fz_page*) mupdf_page->page);

    /* setup text */
    mupdf_page->extracted_text = false;

    mupdf_page->text = fz_new_stext_page(mupdf_page->ctx, mupdf_page->bbox);
    if (mupdf_page->text == NULL) {
        goto error_free;
    }

    epdf_page_set_data(page, mupdf_page);

    /* get page dimensions */
    epdf_page_set_width(page,  mupdf_page->bbox.x1 - mupdf_page->bbox.x0);
    epdf_page_set_height(page, mupdf_page->bbox.y1 - mupdf_page->bbox.y0);

    return EPDF_ERROR_OK;

error_free:

    pdf_page_clear(page, mupdf_page);

    return EPDF_ERROR_UNKNOWN;
}

epdf_error_t
pdf_page_clear(epdf_page_t* page, void* data)
{
    if (page == NULL) {
        return EPDF_ERROR_INVALID_ARGUMENTS;
    }

    mupdf_page_t* mupdf_page         = data;
    epdf_document_t* document     = epdf_page_get_document(page);
    mupdf_document_t* mupdf_document = epdf_document_get_data(document);

    if (mupdf_page != NULL) {
        if (mupdf_page->text != NULL) {
            fz_drop_stext_page(mupdf_page->ctx, mupdf_page->text);
        }

        if (mupdf_page->page != NULL) {
            fz_drop_page(mupdf_document->ctx, mupdf_page->page);
        }

        free(mupdf_page);
    }

    return EPDF_ERROR_UNKNOWN;
}

