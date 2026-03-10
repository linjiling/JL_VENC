
hi_s32 main(hi_s32 argc, hi_char *argv[])
{
    hi_s32 ret;
    hi_u32 index;
    hi_char *end_ptr;

    if (argc < 0x2) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    index = strtoul(argv[1], &end_ptr, 10); /* 10: numberbase */
    if (end_ptr == argv[1] || *end_ptr !='\0') {
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    //sample_sys_signal(sample_venc_handle_sig);

    ret = sample_venc_start_main(index);
    if (ret != HI_SUCCESS) {
        printf("program exit abnormally!\n");
        return HI_FAILURE;
    }

    sample_venc_wait_exit(index);

    printf("program exit normally!\n");

    return ret;
}
