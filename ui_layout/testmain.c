#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h" // for Rectangle, Vector2 (we only use Rectangle here)
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "lib/raylib.lib")

//------------------------------
// Linked List Macros
//------------------------------
#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
        ((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
        CheckNil(nil,p) ? \
        ((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
        ((p)==(l)) ? \
        ((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil,(n)->next)) :\
        (((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))

#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)

//------------------------------
// Axis
//------------------------------
typedef enum Axis2
{
    Axis2_Invalid = -1,
    Axis2_X,
    Axis2_Y,
    Axis2_COUNT,
} Axis2;

typedef enum UI_BoxFlags
{
    UI_BoxFlags_FitChildren = 1,
    UI_BoxFlags_ExpandToParent = 2,
    UI_BoxFlags_MinimumSize = 4,
} UI_BoxFlags;

//------------------------------
// UI_Box
//------------------------------
typedef struct UI_Box UI_Box;
struct UI_Box
{
    char *name;
    uint64_t key;

    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;
    ptrdiff_t child_count;

    UI_BoxFlags flags;

    Rectangle rect;
    Axis2 child_layout_axis;
};

//------------------------------
// Two-Pass Layout
//------------------------------

static void UI_CalcSize(UI_Box *box)
{
    // First, recurse into children to compute their sizes
    for (UI_Box *child = box->first; child; child = child->next)
    {
        UI_CalcSize(child);
    }

    // If this box already has a fixed size, keep it
    float width  = box->rect.width;
    float height = box->rect.height;

    // If no children, nothing else to compute
    if (!box->first) return;

    float total_width = 0, total_height = 0;
    float max_width = 0, max_height = 0;

    // Aggregate child sizes based on layout axis
    if (box->child_layout_axis == Axis2_Y)
    {
        for (UI_Box *child = box->first; child; child = child->next)
        {
            total_height += child->rect.height;
            if (child->rect.width > max_width) max_width = child->rect.width;
        }
        width  = (width  == 0) ? max_width    : width;
        height = (height == 0) ? total_height : height;
    }
    else if (box->child_layout_axis == Axis2_X)
    {
        for (UI_Box *child = box->first; child; child = child->next)
        {
            total_width += child->rect.width;
            if (child->rect.height > max_height) max_height = child->rect.height;
        }
        width  = (width  == 0) ? total_width  : width;
        height = (height == 0) ? max_height   : height;
    }

    // Store computed size back
    box->rect.width = width;
    box->rect.height = height;
}

static void UI_ArrangeBox(UI_Box *box, float abs_x, float abs_y)
{
    // Absolute position of this box
    box->rect.x += abs_x;
    box->rect.y += abs_y;

    float cursor_x = 0;
    float cursor_y = 0;

    // Place children relative to this box
    if (box->child_layout_axis == Axis2_Y)
    {
        for (UI_Box *child = box->first; child; child = child->next)
        {
            child->rect.x = box->rect.x;
            child->rect.y = box->rect.y + cursor_y;
            cursor_y += child->rect.height;

            // Recurse to position descendants
            UI_ArrangeBox(child, 0, 0);
        }
    }
    else if (box->child_layout_axis == Axis2_X)
    {
        for (UI_Box *child = box->first; child; child = child->next)
        {
            child->rect.x = box->rect.x + cursor_x;
            child->rect.y = box->rect.y;
            cursor_x += child->rect.width;

            UI_ArrangeBox(child, 0, 0);
        }
    }
    else
    {
        // No layout axis, just pass transform down
        for (UI_Box *child = box->first; child; child = child->next)
        {
            UI_ArrangeBox(child, box->rect.x, box->rect.y);
        }
    }
}


void UI_LayoutBox(UI_Box *box, int off_x, int off_y)
{
    // Pass 1: bottom-up size calculation
    UI_CalcSize(box);

    // Pass 2: top-down arrangement
    UI_ArrangeBox(box, (float)off_x, (float)off_y);
}


//------------------------------
// Demo
//------------------------------
int main(void)
{
    UI_Box root = {
        .name = "root",
        .rect = {10, 10, 300, 300},
        .child_layout_axis = Axis2_Y,
    };

    UI_Box child1 = {
        .name = "child1",
        .rect = {.width = 50, .height = 50},
        .child_layout_axis = Axis2_X,
        .flags = UI_BoxFlags_FitChildren | UI_BoxFlags_MinimumSize,
    };
    UI_Box child2 = {
        .name = "child2",
        .rect = {.width = 50, .height = 50},
        .child_layout_axis = Axis2_X,
        .flags = UI_BoxFlags_FitChildren | UI_BoxFlags_MinimumSize,
    };
    UI_Box subchild = {
        .name = "subchild",
        .rect = {.width = 10, .height = 10},
    };

    // hierarchy
    child1.parent = &root;
    child2.parent = &root;
    DLLPushBack(root.first, root.last, &child1);
    DLLPushBack(root.first, root.last, &child2);

    subchild.parent = &child2;
    DLLPushBack(child2.first, child2.last, &subchild);

    // run layout
    UI_LayoutBox(&root, 0, 0);

    // print results
    printf("Root:   (%.0f, %.0f, %.0f, %.0f)\n",
            root.rect.x, root.rect.y, root.rect.width, root.rect.height);

    for (UI_Box *c = root.first; c; c = c->next)
    {
        printf(" Child: (%.0f, %.0f, %.0f, %.0f)\n",
                c->rect.x, c->rect.y, c->rect.width, c->rect.height);

        for (UI_Box *sc = c->first; sc; sc = sc->next)
        {
            printf("  Sub:  (%.0f, %.0f, %.0f, %.0f)\n",
                    sc->rect.x, sc->rect.y, sc->rect.width, sc->rect.height);
        }
    }

    InitWindow(800, 600, "Test");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        DrawRectangle(root.rect.x, root.rect.y, root.rect.width, root.rect.height, RED);

        for (UI_Box *c = root.first; c; c = c->next)
        {
            DrawRectangle(c->rect.x, c->rect.y, c->rect.width, c->rect.height, GREEN);

            for (UI_Box *sc = c->first; sc; sc = sc->next)
            {
                DrawRectangle(sc->rect.x, sc->rect.y, sc->rect.width, sc->rect.height, BLUE);
            }
        }

        EndDrawing();
    }

    return 0;
}

