
extern void init_patch_version();
extern void init_patch_score_print();
extern void init_patch_multiscore_print();
extern void init_patch_mapselect();
extern void init_patch_slot_text();
extern void init_patch_misc_text();
extern void init_patch_tooltip();
extern void init_patch_string();
extern void init_patch_message();
extern void init_patch_misc();
extern void init_patch_movie();

static void* _cdecl operator_new(size_t size) {
    return operator new(size);
}
DEF_HOOK(0x006B51D7, operator_new, false, H_new);

static void _cdecl operator_delete(void* ptr) {
    operator delete(ptr);
}
DEF_HOOK(0x006B51CC, operator_delete, false, H_delete);

void init_patch()
{
    H_new.rehook();
    H_delete.rehook();

    init_patch_version();
    init_patch_score_print();
    init_patch_multiscore_print();
    init_patch_mapselect();
    init_patch_slot_text();
    init_patch_tooltip();
    init_patch_misc_text();
    init_patch_string();
    init_patch_message();
    init_patch_misc();
    init_patch_movie();
}
