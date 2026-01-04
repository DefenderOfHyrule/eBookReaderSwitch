#include "LandscapePageLayout.hpp"
#include "common.h"
#include <algorithm>
#include <iostream>

extern fz_context *ctx;

LandscapePageLayout::LandscapePageLayout(fz_document *doc, int current_page):PageLayout(doc, current_page) {
    // recalculate zoom for landscape
    min_zoom = fmin(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    max_zoom = fmax(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    zoom = min_zoom;
    
    // re-render with correct zoom
    render_page_to_texture(_current_page, true);
    reset();
}

void LandscapePageLayout::reset() {
    page_center = fz_make_point(viewport.w / 2, viewport.h / 2);
    
    min_zoom = fmin(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    max_zoom = fmax(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    set_zoom(min_zoom);
};

void LandscapePageLayout::draw_page() {
    float w = page_bounds.x1 * zoom, h = page_bounds.y1 * zoom;
    
    SDL_Rect rect;
    rect.x = page_center.x - w / 2;
    rect.y = page_center.y - h / 2;
    rect.w = w;
    rect.h = h;
    
    SDL_RenderCopyEx(RENDERER, page_texture, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
}

void LandscapePageLayout::move_page(float x, float y) {
    float w = page_bounds.x1 * zoom, h = page_bounds.y1 * zoom;
    
    page_center.x = fmin(fmax(page_center.x + x, h / 2), viewport.w - h / 2);
    page_center.y = fmin(fmax(page_center.y + y, w / 2), viewport.h - w / 2);
}

void LandscapePageLayout::render_page_to_texture(int num, bool reset_zoom) {
    // first let parent do its thing
    PageLayout::render_page_to_texture(num, false);  // always pass false to prevent parent from resetting zoom
    
    // fix zoom values for landscape orientation
    min_zoom = fmin(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    max_zoom = fmax(viewport.w / page_bounds.y1, viewport.h / page_bounds.x1);
    
    if (reset_zoom) {
        zoom = min_zoom;
    } else {
        // bind existing zoom to new bounds
        zoom = fmin(fmax(min_zoom, zoom), max_zoom);
    }
    
    // re-render the texture with correct zoom
    PageLayout::render_page_to_texture(num, false);
}