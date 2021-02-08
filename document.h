#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

/**
 * Open the document
 *
 * @param plugin_manager The epdf instance
 * @param path Path to the document
 * @param password Password of the document or NULL
 * @param error Optional error parameter
 * @return The document object and NULL if an error occurs
 */
epdf_document_t* epdf_document_open(epdf_t* epdf,
    const char* path, const char *uri, const char* password, epdf_error_t*
    error);

/**
 * Free the document
 *
 * @param document
 * @return EPDF_ERROR_OK when no error occurred, otherwise see
 *    epdf_error_t
 */
EPDF_PLUGIN_API epdf_error_t epdf_document_free(epdf_document_t* document);

/**
 * Returns the path of the document
 *
 * @param document The document
 * @return The file path of the document
 */
EPDF_PLUGIN_API const char* epdf_document_get_path(epdf_document_t* document);

/**
 * Returns the URI of the document
 *
 * @param document The document
 * @return The URI of the document
 */
EPDF_PLUGIN_API const char* epdf_document_get_uri(epdf_document_t* document);

/**
 * Returns the basename of the document
 *
 * @param document The document
 * @return The basename of the document
 */
EPDF_PLUGIN_API const char* epdf_document_get_basename(epdf_document_t* document);

/**
 * Returns the SHA256 hash of the document
 *
 * @param document The document
 * @return The SHA256 hash of the document
 */
EPDF_PLUGIN_API const uint8_t* epdf_document_get_hash(epdf_document_t* document);

/**
 * Returns the password of the document
 *
 * @param document The document
 * @return Returns the password of the document
 */
EPDF_PLUGIN_API const char* epdf_document_get_password(epdf_document_t* document);

/**
 * Returns the page at the given index
 *
 * @param document The document
 * @param index The index of the page
 * @return The page or NULL if an error occurred
 */
EPDF_PLUGIN_API epdf_page_t* epdf_document_get_page(epdf_document_t* document, unsigned int index);

/**
 * Returns the number of pages
 *
 * @param document The document
 * @return Number of pages
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_number_of_pages(epdf_document_t* document);

/**
 * Sets the number of pages
 *
 * @param document The document
 * @param number_of_pages Number of pages
 */
EPDF_PLUGIN_API void epdf_document_set_number_of_pages(epdf_document_t* document, unsigned
    int number_of_pages);

/**
 * Returns the current page number
 *
 * @param document The document
 * @return Current page
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_current_page_number(epdf_document_t* document);

/**
 * Sets the number of pages
 *
 * @param document The document
 * @param current_page The current page number
 */
EPDF_PLUGIN_API void epdf_document_set_current_page_number(epdf_document_t* document, unsigned
    int current_page);

/**
 * Returns the X position, as a value relative to the document width (0=left,
 * 1=right).
 *
 * @param document The document
 * @return X adjustment
 */
EPDF_PLUGIN_API double epdf_document_get_position_x(epdf_document_t* document);

/**
 * Returns the Y position as value relative to the document height (0=top,
 * 1=bottom)
 *
 * @param document The document
 * @return Y adjustment
 */
EPDF_PLUGIN_API double epdf_document_get_position_y(epdf_document_t* document);

/**
 * Sets the X position as a value relative to the document width (0=left,
 * 1=right)
 *
 * @param document The document
 * @param position_x the X adjustment
 */
EPDF_PLUGIN_API void epdf_document_set_position_x(epdf_document_t* document, double position_x);

/**
 * Sets the Y position as a value relative to the document height (0=top,
 * 1=bottom)
 *
 * @param document The document
 * @param position_y the Y adjustment
 */
EPDF_PLUGIN_API void epdf_document_set_position_y(epdf_document_t* document, double position_y);

/**
 * Returns the current zoom value of the document
 *
 * @param document The document
 * @return The current zoom value
 */
EPDF_PLUGIN_API double epdf_document_get_zoom(epdf_document_t* document);

/**
 * Returns the current scale value of the document (based on zoom and screen
 * PPI)
 *
 * @param document The document
 * @return The current scale value, in pixels per point
 */
EPDF_PLUGIN_API double epdf_document_get_scale(epdf_document_t* document);

/**
 * Sets the new zoom value of the document
 *
 * @param document The document
 * @param zoom The new zoom value
 */
EPDF_PLUGIN_API void epdf_document_set_zoom(epdf_document_t* document, double zoom);

/**
 * Returns the rotation value of epdf (0..360)
 *
 * @param document The document
 * @return The current rotation value
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_rotation(epdf_document_t* document);

/**
 * Sets the new rotation value
 *
 * @param document The document
 * @param rotation The new rotation value
 */
EPDF_PLUGIN_API void epdf_document_set_rotation(epdf_document_t* document, unsigned int rotation);

/**
 * Returns the adjust mode of the document
 *
 * @param document The document
 * @return The adjust mode
 */
EPDF_PLUGIN_API epdf_adjust_mode_t epdf_document_get_adjust_mode(epdf_document_t* document);

/**
 * Sets the new adjust mode of the document
 *
 * @param document The document
 * @param mode The new adjust mode
 */
EPDF_PLUGIN_API void epdf_document_set_adjust_mode(epdf_document_t* document, epdf_adjust_mode_t mode);

/**
 * Returns the page offset of the document
 *
 * @param document The document
 * @return The page offset
 */
EPDF_PLUGIN_API int epdf_document_get_page_offset(epdf_document_t* document);

/**
 * Sets the new page offset of the document
 *
 * @param document The document
 * @param page_offset The new page offset
 */
EPDF_PLUGIN_API void epdf_document_set_page_offset(epdf_document_t* document, unsigned int page_offset);

/**
 * Returns the private data of the document
 *
 * @param document The document
 * @return The private data or NULL
 */
EPDF_PLUGIN_API void* epdf_document_get_data(epdf_document_t* document);

/**
 * Sets the private data of the document
 *
 * @param document The document
 * @param data The new private data
 */
EPDF_PLUGIN_API void epdf_document_set_data(epdf_document_t* document, void* data);

/**
 * Sets the width of the viewport in pixels.
 *
 * @param[in] document     The document instance
 * @param[in] width        The width of the viewport
 */
void
EPDF_PLUGIN_API epdf_document_set_viewport_width(epdf_document_t* document, unsigned int width);

/**
 * Sets the height of the viewport in pixels.
 *
 * @param[in] document     The document instance
 * @param[in] height       The height of the viewport
 */
void
EPDF_PLUGIN_API epdf_document_set_viewport_height(epdf_document_t* document, unsigned int height);

/**
 * Return the size of the viewport in pixels.
 *
 * @param[in]  document     The document instance
 * @param[out] height,width The width and height of the viewport
 */
void
EPDF_PLUGIN_API epdf_document_get_viewport_size(epdf_document_t* document,
                                   unsigned int *height, unsigned int* width);

/**
 Sets the viewport PPI (pixels per inch: the resolution of the monitor, after
 scaling with the device factor).
 *
 * @param[in] document     The document instance
 * @param[in] height       The viewport PPI
 */
void
EPDF_PLUGIN_API epdf_document_set_viewport_ppi(epdf_document_t* document, double ppi);

/**
 * Return the viewport PPI (pixels per inch: the resolution of the monitor,
 * after scaling with the device factor).
 *
 * @param[in] document     The document instance
 * @return    The viewport PPI
 */
double
EPDF_PLUGIN_API epdf_document_get_viewport_ppi(epdf_document_t* document);

/**
 * Set the device scale factors (e.g. for HiDPI). These are generally integers
 * and equal for x and y. These scaling factors are only used when rendering to
 * the screen.
 *
 * @param[in] x_factor,yfactor The x and y scale factors
 */
void
EPDF_PLUGIN_API epdf_document_set_device_factors(epdf_document_t* document,
                                  double x_factor, double y_factor);
/**
 * Return the current device scale factors (guaranteed to be non-zero).
 *
 * @return The x and y device scale factors
 */
EPDF_PLUGIN_API epdf_device_factors_t
epdf_document_get_device_factors(epdf_document_t* document);

/**
 * Return the size of a cell from the document's layout table in pixels. Assumes
 * that the table is homogeneous (i.e. every cell has the same dimensions). It
 * takes the current scale into account.
 *
 * @param[in]  document     The document instance
 * @param[out] height,width The computed height and width of the cell
 */
EPDF_PLUGIN_API void epdf_document_get_cell_size(epdf_document_t* document,
                                    unsigned int* height, unsigned int* width);

/**
 * Compute the size of the entire document to be displayed in pixels. Takes into
 * account the scale, the layout of the pages, and the padding between them. It
 * should be equal to the allocation of epdf->ui.page_widget once it's shown.
 *
 * @param[in]  document               The document
 * @param[out] height,width           The height and width of the document
 */
EPDF_PLUGIN_API void epdf_document_get_document_size(epdf_document_t* document,
                                        unsigned int* height, unsigned int* width);

/**
 * Sets the layout of the pages in the document
 *
 * @param[in]  document          The document instance
 * @param[in]  page_padding      pixels of padding between pages
 * @param[in]  pages_per_row     number of pages per row
 * @param[in]  first_page_column column of the first page (first column is 1)
 */
EPDF_PLUGIN_API void epdf_document_set_page_layout(epdf_document_t* document, unsigned int page_padding,
                                      unsigned int pages_per_row, unsigned int first_page_column);

/**
 * Returns the padding in pixels between pages
 *
 * @param document The document
 * @return The padding in pixels between pages
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_page_padding(epdf_document_t* document);

/**
 * Returns the number of pages per row
 *
 * @param document The document
 * @return The number of pages per row
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_pages_per_row(epdf_document_t* document);

/**
 * Returns the column for the first page (first column = 1)
 *
 * @param document The document
 * @return The column for the first page
 */
EPDF_PLUGIN_API unsigned int epdf_document_get_first_page_column(epdf_document_t* document);

/**
 * Save the document
 *
 * @param document The document object
 * @param path Path for the saved file
 * @return EPDF_ERROR_OK when no error occurred, otherwise see
 *    epdf_error_t
 */
EPDF_PLUGIN_API epdf_error_t epdf_document_save_as(epdf_document_t* document, const char* path);

/**
 * Save document attachment
 *
 * @param document The document objects
 * @param attachment name of the attachment
 * @param file the target filename
 * @return EPDF_ERROR_OK when no error occurred, otherwise see
 *    epdf_error_t
 */
EPDF_PLUGIN_API epdf_error_t epdf_document_attachment_save(epdf_document_t* document, const char* attachment, const char* file);

#endif // DOCUMENT_H
