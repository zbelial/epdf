#ifndef TYPES_H
#define TYPES_H

#include <mupdf/fitz.h>
#include <mupdf/pdf.h>
/**
 * Adjust mode
 */
typedef enum epdf_adjust_mode_e
{
    EPDF_ADJUST_NONE, /**< No adjustment */
    EPDF_ADJUST_BESTFIT, /**< Adjust to best-fit */
    EPDF_ADJUST_WIDTH, /**< Adjust to width */
    EPDF_ADJUST_INPUTBAR, /**< Focusing the inputbar */
    EPDF_ADJUST_MODE_NUMBER /**< Number of adjust modes */
} epdf_adjust_mode_t;

/**
 * Device scaling structure.
 */
typedef struct epdf_device_factors_s
{
  double x;
  double y;
} epdf_device_factors_t;


/**
 * Document
 */
typedef struct epdf_document_s {
    char* file_path; /**< File path of the document */
    char* uri; /**< URI of the document */
    char* basename; /**< Basename of the document */
    uint8_t hash_sha256[32]; /**< SHA256 hash of the document */
    const char* password; /**< Password of the document */
    unsigned int current_page_number; /**< Current page number */
    unsigned int number_of_pages; /**< Number of pages */
    double zoom; /**< Zoom value */
    unsigned int rotate; /**< Rotation */
    void* data; /**< Custom data */
    epdf_adjust_mode_t adjust_mode; /**< Adjust mode (best-fit, width) */
    int page_offset; /**< Page offset */
    double cell_width; /**< width of a page cell in the document (not transformed by scale and rotation) */
    double cell_height; /**< height of a page cell in the document (not transformed by scale and rotation) */
    unsigned int view_width; /**< width of current viewport */
    unsigned int view_height; /**< height of current viewport */
    double view_ppi; /**< PPI of the current viewport */
    epdf_device_factors_t device_factors; /**< x and y device scale factors (for e.g. HiDPI) */
    unsigned int pages_per_row; /**< number of pages in a row */
    unsigned int first_page_column; /**< column of the first page */
    unsigned int page_padding; /**< padding between pages */
    double position_x; /**< X adjustment */
    double position_y; /**< Y adjustment */

    /**
     * Document pages
     */
    epdf_page_t** pages;

} epdf_document_t;

/**
 * Page
 */
typedef struct epdf_page_s {
    double height; /**< Page height */
    double width; /**< Page width */
    unsigned int index; /**< Page number */
    void* data; /**< Custom data */
    bool visible; /**< Page is visible */
    epdf_document_t* document; /**< Document */
} epdf_page_t;

/**
 * Error types
 */
typedef enum epdf_plugin_error_e
{
  EPDF_ERROR_OK, /**< No error occurred */
  EPDF_ERROR_UNKNOWN, /**< An unknown error occurred */
  EPDF_ERROR_OUT_OF_MEMORY, /**< Out of memory */
  EPDF_ERROR_NOT_IMPLEMENTED, /**< The called function has not been implemented */
  EPDF_ERROR_INVALID_ARGUMENTS, /**< Invalid arguments have been passed */
  EPDF_ERROR_INVALID_PASSWORD /**< The provided password is invalid */
} epdf_error_t;


typedef struct mupdf_document_s
{
  fz_context* ctx; /**< Context */
  fz_document* document; /**< mupdf document */
} mupdf_document_t;

typedef struct mupdf_page_s
{
  fz_page* page; /**< Reference to the mupdf page */
  fz_context* ctx; /**< Context */
  fz_stext_page* text; /**< Page text */
  fz_rect bbox; /**< Bbox */
  bool extracted_text; /**< If text has already been extracted */
} mupdf_page_t;



#endif
