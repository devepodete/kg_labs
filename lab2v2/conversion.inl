#pragma once


mm::vec2 normalizedToWindow(const mm::vec2 &normalizedCords, int width, int height) {
    return mm::vec2((double)width/2.0 * (1+normalizedCords[0]),
                    (double)height/2.0 * (1+normalizedCords[1]));
}
