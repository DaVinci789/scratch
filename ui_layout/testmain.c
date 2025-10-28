#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h" // for Rectangle, Vector2 (we only use Rectangle here)
#include "base.c"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "lib/raylib.lib")

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

typedef uint32_t OS_Modifiers;
enum
{
    OS_Modifier_Ctrl  = (1<<0),
    OS_Modifier_Shift = (1<<1),
    OS_Modifier_Alt   = (1<<2),
};

typedef struct
{
    u64 u64;
} UI_Key;

typedef uint32_t UI_SignalFlags;
enum
{
    // rjf: mouse press -> box was pressed while hovering
    UI_SignalFlag_LeftPressed         = (1<<0),
    UI_SignalFlag_MiddlePressed       = (1<<1),
    UI_SignalFlag_RightPressed        = (1<<2),

    // rjf: dragging -> box was previously pressed, user is still holding button
    UI_SignalFlag_LeftDragging        = (1<<3),
    UI_SignalFlag_MiddleDragging      = (1<<4),
    UI_SignalFlag_RightDragging       = (1<<5),

    // rjf: double-dragging -> box was previously double-clicked, user is still holding button
    UI_SignalFlag_LeftDoubleDragging  = (1<<6),
    UI_SignalFlag_MiddleDoubleDragging= (1<<7),
    UI_SignalFlag_RightDoubleDragging = (1<<8),

    // rjf: triple-dragging -> box was previously triple-clicked, user is still holding button
    UI_SignalFlag_LeftTripleDragging  = (1<<9),
    UI_SignalFlag_MiddleTripleDragging= (1<<10),
    UI_SignalFlag_RightTripleDragging = (1<<11),

    // rjf: released -> box was previously pressed & user released, in or out of bounds
    UI_SignalFlag_LeftReleased        = (1<<12),
    UI_SignalFlag_MiddleReleased      = (1<<13),
    UI_SignalFlag_RightReleased       = (1<<14),

    // rjf: clicked -> box was previously pressed & user released, in bounds
    UI_SignalFlag_LeftClicked         = (1<<15),
    UI_SignalFlag_MiddleClicked       = (1<<16),
    UI_SignalFlag_RightClicked        = (1<<17),

    // rjf: double clicked -> box was previously clicked, pressed again
    UI_SignalFlag_LeftDoubleClicked   = (1<<18),
    UI_SignalFlag_MiddleDoubleClicked = (1<<19),
    UI_SignalFlag_RightDoubleClicked  = (1<<20),

    // rjf: triple clicked -> box was previously clicked twice, pressed again
    UI_SignalFlag_LeftTripleClicked   = (1<<21),
    UI_SignalFlag_MiddleTripleClicked = (1<<22),
    UI_SignalFlag_RightTripleClicked  = (1<<23),

    // rjf: keyboard pressed -> box had focus, user activated via their keyboard
    UI_SignalFlag_KeyboardPressed     = (1<<24),

    // rjf: passive mouse info
    UI_SignalFlag_Hovering            = (1<<25), // hovering specifically this box
    UI_SignalFlag_MouseOver           = (1<<26), // mouse is over, but may be occluded

    // rjf: committing state changes via user interaction
    UI_SignalFlag_Commit              = (1<<27),

    // rjf: high-level combos
    UI_SignalFlag_Pressed = UI_SignalFlag_LeftPressed|UI_SignalFlag_KeyboardPressed,
    UI_SignalFlag_Released = UI_SignalFlag_LeftReleased,
    UI_SignalFlag_Clicked = UI_SignalFlag_LeftClicked|UI_SignalFlag_KeyboardPressed,
    UI_SignalFlag_DoubleClicked = UI_SignalFlag_LeftDoubleClicked,
    UI_SignalFlag_TripleClicked = UI_SignalFlag_LeftTripleClicked,
    UI_SignalFlag_Dragging = UI_SignalFlag_LeftDragging,
};

#define ui_pressed(s)        !!((s).f&UI_SignalFlag_Pressed)
#define ui_clicked(s)        !!((s).f&UI_SignalFlag_Clicked)
#define ui_released(s)       !!((s).f&UI_SignalFlag_Released)
#define ui_double_clicked(s) !!((s).f&UI_SignalFlag_DoubleClicked)
#define ui_triple_clicked(s) !!((s).f&UI_SignalFlag_TripleClicked)
#define ui_middle_clicked(s) !!((s).f&UI_SignalFlag_MiddleClicked)
#define ui_right_clicked(s)  !!((s).f&UI_SignalFlag_RightClicked)
#define ui_dragging(s)       !!((s).f&UI_SignalFlag_Dragging)
#define ui_hovering(s)       !!((s).f&UI_SignalFlag_Hovering)
#define ui_mouse_over(s)     !!((s).f&UI_SignalFlag_MouseOver)
#define ui_committed(s)      !!((s).f&UI_SignalFlag_Commit)

typedef u64 UI_BoxFlags;
//{
//- rjf: interaction
# define UI_BoxFlag_MouseClickable            (UI_BoxFlags)(1ull<<0)
# define UI_BoxFlag_KeyboardClickable         (UI_BoxFlags)(1ull<<1)
# define UI_BoxFlag_DropSite                  (UI_BoxFlags)(1ull<<2)
# define UI_BoxFlag_ClickToFocus              (UI_BoxFlags)(1ull<<3)
# define UI_BoxFlag_Scroll                    (UI_BoxFlags)(1ull<<4)
# define UI_BoxFlag_ViewScrollX               (UI_BoxFlags)(1ull<<5)
# define UI_BoxFlag_ViewScrollY               (UI_BoxFlags)(1ull<<6)
# define UI_BoxFlag_ViewClampX                (UI_BoxFlags)(1ull<<7)
# define UI_BoxFlag_ViewClampY                (UI_BoxFlags)(1ull<<8)
# define UI_BoxFlag_FocusHot                  (UI_BoxFlags)(1ull<<9)
# define UI_BoxFlag_FocusActive               (UI_BoxFlags)(1ull<<10)
# define UI_BoxFlag_FocusHotDisabled          (UI_BoxFlags)(1ull<<11)
# define UI_BoxFlag_FocusActiveDisabled       (UI_BoxFlags)(1ull<<12)
# define UI_BoxFlag_DefaultFocusNavX          (UI_BoxFlags)(1ull<<13)
# define UI_BoxFlag_DefaultFocusNavY          (UI_BoxFlags)(1ull<<14)
# define UI_BoxFlag_DefaultFocusEdit          (UI_BoxFlags)(1ull<<15)
# define UI_BoxFlag_FocusNavSkip              (UI_BoxFlags)(1ull<<16)
# define UI_BoxFlag_DisableTruncatedHover     (UI_BoxFlags)(1ull<<17)
# define UI_BoxFlag_Disabled                  (UI_BoxFlags)(1ull<<18)

//- rjf: layout
# define UI_BoxFlag_FloatingX                 (UI_BoxFlags)(1ull<<19)
# define UI_BoxFlag_FloatingY                 (UI_BoxFlags)(1ull<<20)
# define UI_BoxFlag_FixedWidth                (UI_BoxFlags)(1ull<<21)
# define UI_BoxFlag_FixedHeight               (UI_BoxFlags)(1ull<<22)
# define UI_BoxFlag_AllowOverflowX            (UI_BoxFlags)(1ull<<23)
# define UI_BoxFlag_AllowOverflowY            (UI_BoxFlags)(1ull<<24)
# define UI_BoxFlag_SkipViewOffX              (UI_BoxFlags)(1ull<<25)
# define UI_BoxFlag_SkipViewOffY              (UI_BoxFlags)(1ull<<26)

//- rjf: appearance / animation
# define UI_BoxFlag_DrawDropShadow            (UI_BoxFlags)(1ull<<27)
# define UI_BoxFlag_DrawBackgroundBlur        (UI_BoxFlags)(1ull<<28)
# define UI_BoxFlag_DrawBackground            (UI_BoxFlags)(1ull<<29)
# define UI_BoxFlag_DrawBorder                (UI_BoxFlags)(1ull<<30)
# define UI_BoxFlag_DrawSideTop               (UI_BoxFlags)(1ull<<31)
# define UI_BoxFlag_DrawSideBottom            (UI_BoxFlags)(1ull<<32)
# define UI_BoxFlag_DrawSideLeft              (UI_BoxFlags)(1ull<<33)
# define UI_BoxFlag_DrawSideRight             (UI_BoxFlags)(1ull<<34)
# define UI_BoxFlag_DrawText                  (UI_BoxFlags)(1ull<<35)
# define UI_BoxFlag_DrawTextFastpathCodepoint (UI_BoxFlags)(1ull<<36)
# define UI_BoxFlag_DrawTextWeak              (UI_BoxFlags)(1ull<<37)
# define UI_BoxFlag_DrawHotEffects            (UI_BoxFlags)(1ull<<38)
# define UI_BoxFlag_DrawActiveEffects         (UI_BoxFlags)(1ull<<39)
# define UI_BoxFlag_DrawOverlay               (UI_BoxFlags)(1ull<<40)
# define UI_BoxFlag_DrawBucket                (UI_BoxFlags)(1ull<<41)
# define UI_BoxFlag_Clip                      (UI_BoxFlags)(1ull<<42)
# define UI_BoxFlag_AnimatePosX               (UI_BoxFlags)(1ull<<43)
# define UI_BoxFlag_AnimatePosY               (UI_BoxFlags)(1ull<<44)
# define UI_BoxFlag_DisableTextTrunc          (UI_BoxFlags)(1ull<<45)
# define UI_BoxFlag_DisableIDString           (UI_BoxFlags)(1ull<<46)
# define UI_BoxFlag_DisableFocusBorder        (UI_BoxFlags)(1ull<<47)
# define UI_BoxFlag_DisableFocusOverlay       (UI_BoxFlags)(1ull<<48)
# define UI_BoxFlag_HasDisplayString          (UI_BoxFlags)(1ull<<49)
# define UI_BoxFlag_HasFuzzyMatchRanges       (UI_BoxFlags)(1ull<<50)
# define UI_BoxFlag_RoundChildrenByParent     (UI_BoxFlags)(1ull<<51)
# define UI_BoxFlag_SquishAnchored            (UI_BoxFlags)(1ull<<52)

//- rjf: debug
# define UI_BoxFlag_Debug                     (UI_BoxFlags)(1ull<<53)

//- rjf: bundles
# define UI_BoxFlag_Clickable           (UI_BoxFlag_MouseClickable|UI_BoxFlag_KeyboardClickable)
# define UI_BoxFlag_DefaultFocusNav     (UI_BoxFlag_DefaultFocusNavX|UI_BoxFlag_DefaultFocusNavY|UI_BoxFlag_DefaultFocusEdit)
# define UI_BoxFlag_Floating            (UI_BoxFlag_FloatingX|UI_BoxFlag_FloatingY)
# define UI_BoxFlag_FixedSize           (UI_BoxFlag_FixedWidth|UI_BoxFlag_FixedHeight)
# define UI_BoxFlag_AllowOverflow       (UI_BoxFlag_AllowOverflowX|UI_BoxFlag_AllowOverflowY)
# define UI_BoxFlag_AnimatePos          (UI_BoxFlag_AnimatePosX|UI_BoxFlag_AnimatePosY)
# define UI_BoxFlag_ViewScroll          (UI_BoxFlag_ViewScrollX|UI_BoxFlag_ViewScrollY)
# define UI_BoxFlag_ViewClamp           (UI_BoxFlag_ViewClampX|UI_BoxFlag_ViewClampY)
# define UI_BoxFlag_DisableFocusEffects (UI_BoxFlag_DisableFocusBorder|UI_BoxFlag_DisableFocusOverlay)

UI_Key ui_key_zero()
{
    return (UI_Key) {0};
}

typedef struct UI_Box UI_Box;
typedef void (UI_BoxCustomDrawFunction)(UI_Box *box, void *user_data);

//------------------------------
// UI_Box
//------------------------------
struct UI_Box
{
    UI_Key key;

    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;
    ptrdiff_t child_count;

    Str string;
    UI_BoxFlags flags;
    Vector2 min_size;
    Axis2 child_layout_axis;
    Color background_color;
    Color text_color;
    Color border_color;
    UI_BoxCustomDrawFunction *custom_draw;
    void *custom_draw_user_data;

    Vector2 drag_begin;

    float active_t;
    float active_t_target;

    Rectangle rect;
};

typedef struct UI_Signal UI_Signal;
struct UI_Signal
{
    UI_Box *box;
    OS_Modifiers event_flags;
    Vector2 scroll;
    UI_SignalFlags f;
};

// Hashmap
typedef struct UI_BoxMap UI_BoxMap; // rename as needed
struct UI_BoxMap
{
    UI_BoxMap *child[4];
    UI_Key     key;
    UI_Box     value;
};

typedef struct UI_SignalMap UI_SignalMap; 
struct UI_SignalMap
{
    UI_SignalMap *child[4];
    UI_Key        key;
    UI_Signal     value;
};

UI_Box *UI_Lookup(UI_BoxMap **env, UI_Key key, Arena *a)
{
    UI_Box *result = 0;
    for (uint64_t h = key.u64; *env; h <<= 2) 
    {
        if (key.u64 == (*env)->key.u64)
        {
            result = &(*env)->value;
        }
        env = &(*env)->child[h>>62];
    }
    if (!result && a)
    {
        *env = new(a, 1, UI_BoxMap);
        (*env)->key = key;
        result = &(*env)->value;
    }
    return result;
}

UI_Signal *UI_SignalLookup(UI_SignalMap **env, UI_Key key, Arena *a)
{
    UI_Signal *result = 0;
    for (uint64_t h = key.u64; *env; h <<= 2) 
    {
        if (key.u64 == (*env)->key.u64)
        {
            result = &(*env)->value;
        }
        env = &(*env)->child[h>>62];
    }
    if (!result && a)
    {
        *env = new(a, 1, UI_SignalMap);
        (*env)->key = key;
        result = &(*env)->value;
    }
    return result;
}

//------------------------------
// Two-Pass Layout
//------------------------------

static void UI_CalcSize(UI_Box *box)
{
    // First, recurse into children to compute their sizes
    for (UI_Box *child = box->first; child; child = child->next)
    {
        UI_CalcSize(child);
        box->child_count += 1;
    }

    float width  = box->min_size.x;
    float height = box->min_size.y;

    if (box->first) // only if box has children
    {
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
            width  = max_width;
            height = total_height;
        }
        else if (box->child_layout_axis == Axis2_X)
        {
            for (UI_Box *child = box->first; child; child = child->next)
            {
                total_width += child->rect.width;
                if (child->rect.height > max_height) max_height = child->rect.height;
            }
            width  = total_width;
            height = max_height;
        }
    }

    // Store computed size back
    box->rect.width = width > box->min_size.x ? width : box->min_size.x;
    box->rect.height = height > box->min_size.y ? height : box->min_size.y;
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

void UI_LayoutBox(UI_Box *box, float off_x, float off_y)
{
    // Pass 1: bottom-up size calculation
    UI_CalcSize(box);

    // Pass 2: top-down arrangement
    UI_ArrangeBox(box, off_x, off_y);
}

typedef struct UI_Node UI_Node;
struct UI_Node
{
    UI_Box  *box;
    UI_Node *next;
};

typedef struct
{
    UI_Box *current_parent;
    UI_Node *parent_stack;
    UI_BoxMap *map;
    UI_SignalMap *signal_map;
    Arena *build_arena;
    Arena *map_arena;
    bool click_available;
    UI_Key active_box_key;
} UI_State;

void ui_push_parent(UI_State *ctx, UI_Box *box)
{
    // Allocate a link node from the arena
    UI_Node *node = new(ctx->build_arena, 1, UI_Node);
    node->box = ctx->current_parent;  // remember the previous parent
    SLLStackPush(ctx->parent_stack, node);  // push node on stack

    ctx->current_parent = box;  // set new parent
}

void ui_pop_parent(UI_State *ctx)
{
    UI_Node *node = ctx->parent_stack;
    SLLStackPop(ctx->parent_stack);     // pop
    ctx->current_parent = node->box;    // restore parent
}

void ui_box_make(UI_State *ctx, UI_Box *box)
{
    box->parent = ctx->current_parent;
    if (!ctx->current_parent)
    {
        ui_push_parent(ctx, box);
    }
    else
    {
        DLLPushBack(ctx->current_parent->first, ctx->current_parent->last, box);
    }
}

#define DeferLoop(begin, end) for(int _i_ = ((begin), 0); _i_ == 0; (_i_ += 1), (end))
#define UI_Parent(ctx, n) DeferLoop(ui_push_parent(ctx, n), ui_pop_parent(ctx))

UI_Signal ui_signal_from_box(UI_State *ctx, UI_Box *box)
{
    UI_Signal result = {0};
    UI_Signal *cached = UI_SignalLookup(&ctx->signal_map, box->key, 0);
    if (cached)
    {
        result = *cached;
    }
    else
    {
        result.box = box;
        // box = UI_Lookup(&ctx->map, box->key, ctx->map_arena);
        if (ctx->click_available)
        {
            if (CheckCollisionPointRec(GetMousePosition(), box->rect))
            {
                result.f |= UI_SignalFlag_Pressed;
                ctx->click_available = false;
                ctx->active_box_key = box->key;
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), box->rect))
        {
            result.f |= UI_SignalFlag_Hovering;
        }
        if (ctx->active_box_key.u64 == box->key.u64)
        {
            result.f |= UI_SignalFlag_Dragging;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            ctx->active_box_key = (UI_Key) {0};
        }
        *UI_SignalLookup(&ctx->signal_map, box->key, ctx->build_arena) = result;
    }
    return result;
}

void UI_UpdateBoxMap(UI_State *ctx, UI_Box box)
{
    // Insert or update this box in the map
    *UI_Lookup(&ctx->map, box.key, ctx->map_arena) = box;

    // Recurse through children
    for (UI_Box *child = box.first; child; child = child->next)
    {
        UI_UpdateBoxMap(ctx, *child);
    }
}


// Go from building boxes manually to building boxes with functions
UI_Box *ui_build_box_from_key(UI_State *ctx, UI_BoxFlags flags, UI_Key key)
{
    UI_Box *result = 0;
    if (key.u64 != 0)
    {
        result = UI_Lookup(&ctx->map, key, ctx->map_arena);
    }
    else
    {
        result = new(ctx->build_arena, 1, UI_Box);
    }
    result->flags = flags;
    result->key = key;
    ui_box_make(ctx, result);
    return result;
}

UI_Box *ui_build_box_from_string(UI_State *ctx, UI_BoxFlags flags, Str string)
{
    u64 hash = hash64(string);
    UI_Key key = (UI_Key) {
        .u64 = hash,
    };
    UI_Box *result = ui_build_box_from_key(ctx, flags, key);
    result->string = string;
    return result;
}

void UI_Draw(UI_Box *root)
{
    if (root)
    {
        // Pass 1: draw current layer (siblings)
        for (UI_Box *box = root; box; box = box->next)
        {
            if (box->custom_draw)
            {
                box->custom_draw(box, box->custom_draw_user_data);
            }
            else
            {
                DrawRectangle(box->rect.x, box->rect.y, box->rect.width, box->rect.height, box->border_color);
                if (box->flags & UI_BoxFlag_DrawText)
                {
                    DrawText(box->string.data, box->rect.x, box->rect.y, 20, BLACK);
                }
            }
        }

        // Pass 2: recurse into next layer (children of each)
        for (UI_Box *box = root; box; box = box->next)
        {
            if (box->first)
            {
                UI_Draw(box->first);
            }
        }
    }
}

void draw_button(UI_Box *box, void *user_data)
{
    UI_Signal signal = ui_signal_from_box(user_data, box);
    if (ui_pressed(signal))
    {
        box->active_t_target = 0.12;
    }
    if (ui_pressed(signal) || box->active_t_target > 0.0)
    {
        DrawRectangleRec(box->rect, PURPLE);
        box->active_t += GetFrameTime();
    }
    else if (ui_hovering(signal))
    {
        DrawRectangleRec(box->rect, ORANGE);
    }
    else
    {
        DrawRectangleRec(box->rect, GREEN);
    }
    if (box->active_t >= box->active_t_target)
    {
        box->active_t_target = 0.0;
        box->active_t = 0.0;
    }
}

UI_Box *ui_spacer(UI_State *ctx, Axis2 axis, int amt)
{
    UI_Box *spacer = ui_build_box_from_key(ctx, 0, ui_key_zero());
    if (axis == Axis2_X)
    {
        spacer->min_size.x = amt;
    }
    else if (axis == Axis2_Y)
    {
        spacer->min_size.y = amt;
    }
    return spacer;
}

UI_Signal UI_Button(UI_State *ctx, Str text)
{
    UI_Box *button = ui_build_box_from_string(ctx, 0, text);
    button->custom_draw = &draw_button;
    button->custom_draw_user_data = ctx;
    button->child_layout_axis = Axis2_X;

    Font default_font = GetFontDefault();
    Vector2 size = MeasureTextEx(default_font, text.data, 20, 2);
    // button->min_size = size;

#define UI_Padding(ctx, axis, amt) DeferLoop(ui_spacer(ctx, axis, amt), ui_spacer(ctx, axis, amt))

    UI_Parent(ctx, button)
    {
        ui_spacer(ctx, Axis2_X, 10);
        // UI_Box *spacer = ui_build_box_from_key(ctx, 0, ui_key_zero());
        // spacer->min_size.x = 10;

        UI_Box *spacer = ui_build_box_from_key(ctx, 0, ui_key_zero());
        spacer->child_layout_axis = Axis2_Y;

        UI_Parent(ctx, spacer)
        {
            ui_spacer(ctx, Axis2_Y, 10);

            // UI_Box *spacer2 = ui_build_box_from_key(ctx, 0, ui_key_zero());
            // spacer2->min_size.y = 10;

            UI_Box *textbox = ui_build_box_from_key(ctx, UI_BoxFlag_DrawText, ui_key_zero());
            textbox->min_size = size;
            textbox->string = text;

            // spacer2 = ui_build_box_from_key(ctx, 0, ui_key_zero());
            // spacer2->min_size.y = 10;

            ui_spacer(ctx, Axis2_Y, 10);
        }

        // spacer = ui_build_box_from_key(ctx, 0, ui_key_zero());
        // spacer->min_size.x = 20;
        ui_spacer(ctx, Axis2_X, 10);
    }

    return ui_signal_from_box(ctx, button);
}


//------------------------------
// Demo
//------------------------------
int main(void)
{
    Arena perm = {0};
    char *perm_base = malloc(1 << 15);
    perm.beg = perm_base;
    perm.end = perm.beg + (1 << 15);

    char *scratch_base = malloc(1 << 15);
    _arena_scratch.beg = scratch_base;
    _arena_scratch.end = _arena_scratch.beg + (1 << 15);

    char *map_base = malloc(1 << 15);

    UI_State ctx = {0};
    ctx.build_arena = &perm; // temp arena
    Arena map_arena = (Arena) {map_base, map_base + (1 << 15)};
    ctx.map_arena   = &map_arena;

    InitWindow(800, 600, "Test");
    while (!WindowShouldClose())
    {
        { // UI Begin
            ctx.click_available = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            ctx.current_parent = 0;
            ctx.parent_stack = 0;
            ctx.signal_map = 0;
            perm.beg = perm_base;
        }

        UI_Box *root = ui_build_box_from_string(&ctx, 0, S("root"));
        root->min_size = (Vector2) {300, 300};
        root->border_color = RED;
        root->child_layout_axis = Axis2_Y;

        UI_Parent(&ctx, root) {
            if (ui_pressed(UI_Button(&ctx, S("Test Button"))))
            {
                printf("You pressed the button!\n");
            }
            if (ui_pressed(UI_Button(&ctx, S("Test Button 2"))))
            {
                printf("You pressed the button 2!\n");
            }
            if (ui_pressed(UI_Button(&ctx, S("Test Button 3"))))
            {
                printf("You pressed the button 3!\n");
            }
            if (ui_pressed(UI_Button(&ctx, S("Test Button 4"))))
            {
                printf("You pressed the button 4!\n");
            }
            if (ui_pressed(UI_Button(&ctx, S("Test Button 5"))))
            {
                printf("You pressed the button 5!\n");
            }
            if (ui_pressed(UI_Button(&ctx, S("Test Button 6"))))
            {
                printf("You pressed the button 6!\n");
            }
#if 0
            UI_Box *child1 = ui_build_box_from_string(&ctx, 0, S("child1"));
            child1->border_color = GREEN;
            child1->custom_draw = &draw_button;
            child1->custom_draw_user_data = &ctx;

            Font default_font = GetFontDefault();
            Vector2 size = MeasureTextEx(default_font, child1->string.data, 20, 2);
            child1->min_size = size;

            UI_Parent(&ctx, child1) {
                UI_Box *subchild = ui_build_box_from_string(&ctx, 0, S("subchild"));
                subchild->min_size = (Vector2) {10, 10};
                subchild->border_color = BLUE;

                if (ui_clicked(ui_signal_from_box(&ctx, subchild)))
                {
                    printf("Howdy, from the subchild!\n");
                }
            }
            if (ui_clicked(ui_signal_from_box(&ctx, child1)))
            {
                printf("Hello, from the ui!\n");
            }

            UI_Box *child2 = ui_build_box_from_string(&ctx, 0, S("child2"));
            child2->min_size = (Vector2) {50, 50};
            child2->border_color = GREEN;

            UI_Box *box = ui_build_box_from_string(&ctx, 0, S("Builder test!"));
            box->min_size = (Vector2) {.x = 100, .y = 100};
            box->border_color = GREEN;
#endif
        }

        Vector2 mouse = GetMousePosition();

        if (ui_pressed(ui_signal_from_box(&ctx, root)))
        {
            // store the offset between mouse and box position
            root->drag_begin.x = mouse.x - root->rect.x;
            root->drag_begin.y = mouse.y - root->rect.y;
        }

        if (ui_dragging(ui_signal_from_box(&ctx, root)))
        {
            // maintain that offset while dragging
            root->rect.x = mouse.x - root->drag_begin.x;
            root->rect.y = mouse.y - root->drag_begin.y;
        }

        { // UI End
            UI_LayoutBox(root, 0, 0);
            UI_UpdateBoxMap(&ctx, *root);
        }


        BeginDrawing();
        ClearBackground(BLACK);
        UI_Draw(root);
        EndDrawing();
    }

    free(perm_base);
    free(map_base);
    free(scratch_base);

    return 0;
}

