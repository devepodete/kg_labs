//
// Created by zero on 27/10/2020.
//


#pragma once

#include "matrices.hpp"

/// Convert normalized screen coordinates to window coordinates
mm::vec2 normalizedToWindow(const mm::vec2 &normalizedCords, int width, int height);

#include "conversion.inl"