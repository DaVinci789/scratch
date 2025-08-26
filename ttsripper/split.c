int toint(char *str, int len)
{
    if (len <= 0) return 0;

    int negative = 0;
    int i = 0;

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }

    int result = 0;
    for (; i < len; i++) {
        char c = str[i];
        if (c < '0' || c > '9') break;  // stop at first non-digit

        // Working in negative to match append_long logic
        result = result * 10 - (c - '0');
    }

    return negative ? result : -result;
}

int split(char *path, int tiles_x, int tiles_y)
{
    Image src = LoadImage(path);
    if (src.data == NULL) return 0;

    int piece_width = src.width / tiles_x;
    int piece_height = src.height / tiles_y;

    int i = 0;
    for (int y = 0; y < tiles_y; y++)
    {
        for (int x = 0; x < tiles_x; x++)
        {
            rlRectangle rect = {
                .x = x * piece_width,
                .y = y * piece_height,
                .width = piece_width,
                .height = piece_height
            };

            Image piece = ImageFromImage(src, rect);

            char filename[1024];
            snprintf(filename, sizeof(filename), "piece%d.png", i++);
            TraceLog(LOG_INFO, "Saved %s", filename);

            ExportImage(piece, filename);
            UnloadImage(piece);
        }
    }

    UnloadImage(src);
    return tiles_x * tiles_y;
}
