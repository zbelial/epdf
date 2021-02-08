#ifndef PAGE_H
#define PAGE_H

#include <cairo.h>

#include "types.h"

/**
 * Get the page object
 *
 * @param document The document
 * @param index Page number
 * @param error Optional error
 * @return Page object or NULL if an error occurred
 */
EPDF_PLUGIN_API epdf_page_t* epdf_page_new(epdf_document_t* document, unsigned int
    index, epdf_error_t* error);

/**
 * Frees the page object
 *
 * @param page The page object
 * @return EPDF_ERROR_OK when no error occurred, otherwise see
 *    epdf_error_t
 */
EPDF_PLUGIN_API epdf_error_t epdf_page_free(epdf_page_t* page);

/**
 * Returns the associated document
 *
 * @param page The page object
 * @return The associated document
 * @return NULL if an error occurred
 */
EPDF_PLUGIN_API epdf_document_t* epdf_page_get_document(epdf_page_t* page);

/**
 * Returns the set id of the page
 *
 * @param page The page object
 * @return The id of the page
 */
EPDF_PLUGIN_API unsigned int epdf_page_get_index(epdf_page_t* page);

/**
 * Returns the width of the page
 *
 * @param page The page object
 * @return Width of the page
 * @return -1 If an error occurred
 */
EPDF_PLUGIN_API double epdf_page_get_width(epdf_page_t* page);

/**
 * Sets the new width of the page
 *
 * @param page The page object
 * @param width The new width of the page
 */
EPDF_PLUGIN_API void epdf_page_set_width(epdf_page_t* page, double width);

/**
 * Returns the height of the page
 *
 * @param page The page object
 * @return Height of the page
 * @return -1 If an error occurred
 */
EPDF_PLUGIN_API double epdf_page_get_height(epdf_page_t* page);

/**
 * Sets the new height of the page
 *
 * @param page The page object
 * @param height The new height of the page
 */
EPDF_PLUGIN_API void epdf_page_set_height(epdf_page_t* page, double height);

/**
 * Returns the visibility of the page
 *
 * @param page The page object
 * @return true if the page is visible
 * @return false if the page is hidden
 */
EPDF_PLUGIN_API bool epdf_page_get_visibility(epdf_page_t* page);

/**
 * Sets the visibility of the page
 *
 * @param page The page object
 * @param visibility The new visibility value
 */
EPDF_PLUGIN_API void epdf_page_set_visibility(epdf_page_t* page, bool visibility);

/**
 * Returns the custom data
 *
 * @param page The page object
 * @return The custom data or NULL
 */
EPDF_PLUGIN_API void* epdf_page_get_data(epdf_page_t* page);

/**
 * Sets the custom data
 *
 * @param page The page object
 * @param data The custom data
 */
EPDF_PLUGIN_API void epdf_page_set_data(epdf_page_t* page, void* data);

/**
 * Get image
 *
 * @param page Page
 * @param image Image identifier
 * @param error Set to an error value (see \ref epdf_error_t) if an
 *   error occurred
 * @return The cairo image surface or NULL if an error occurred
 */
EPDF_PLUGIN_API cairo_surface_t* epdf_page_image_get_cairo(epdf_page_t* page, epdf_image_t* image, epdf_error_t* error);

/**
 * Get text for selection
 * @param page Page
 * @param rectangle Selection
 * @param error Set to an error value (see \ref epdf_error_t) if an error
 * occurred
 * @return The selected text (needs to be deallocated with g_free)
 */
EPDF_PLUGIN_API char* epdf_page_get_text(epdf_page_t* page, epdf_rectangle_t rectangle, epdf_error_t* error);

/**
 * Render page
 *
 * @param page The page object
 * @param cairo Cairo object
 * @param printing render for printing
 * @return EPDF_ERROR_OK when no error occurred, otherwise see
 *    epdf_error_t
 */
EPDF_PLUGIN_API epdf_error_t epdf_page_render(epdf_page_t* page, cairo_t* cairo, bool printing);

/**
 * Get page label. Note that the page label might not exist, in this case NULL
 * is returned.
 *
 * @param page Page
 * @param error Set to an error value (see \ref epdf_Error_t) if an error
 *    occurred.
 * @return Page label
 */
EPDF_PLUGIN_API char* epdf_page_get_label(epdf_page_t* page, epdf_error_t* error);

#endif // PAGE_H
