#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

#include "base.c"

#define UI_Parent(v) DeferLoop(ui_push_parent(v), ui_pop_parent(v))

typedef uint64_t UI_BoxFlags;
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
//}

typedef enum Axis2
{
    Axis2_Invalid = -1,
    Axis2_X,
    Axis2_Y,
    Axis2_COUNT,
} Axis2;
#define axis2_flip(a) ((Axis2)(!(a)))

typedef struct UI_Box UI_Box;
struct UI_Box
{
    uint64_t key;

    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;
    ptrdiff_t child_count;

    UI_BoxFlags flags;
    Rectangle rect;
    Str string;
    Axis2 child_layout_axis;

    float hot_t;
    float active_t;
};

typedef struct
{
    UI_Box *box;
    Vector2 mouse;
    Vector2 drag_delta;
    bool clicked;
    bool double_clicked;
    bool right_clicked;
    bool pressed;
    bool released;
    bool dragging;
    bool hovering;
} UI_Comm;

UI_Comm UI_CommFromBox(UI_Box *box);

int main(void)
{
    UI_Box root = (UI_Box) {
        .rect = (Rectangle) {
            .x = 10,
            .y = 10,
            .width = 10,
            .height = 10,
        },
        .string = S("Root!"),
        .child_layout_axis = Axis2_Y,
    };

    UI_Box child1 = (UI_Box) {
        .rect = (Rectangle) {
            .width  = 50,
            .height = 50,
        },
        .string = S("Child1!"),
    };
    UI_Box child2 = (UI_Box) {
        .rect = (Rectangle) {
            .width  = 70,
            .height = 70,
        },
        .string = S("Child2!"),
    };
    
    child1.parent = &root;
    child2.parent = &root;
    DLLPushBack(root.first, root.last, &child1);
    DLLPushBack(root.first, root.last, &child2);

    for (UI_Box *start = &root; *start; start = start->next)
    {
    }
    return 0;
}
