#ifndef VEDU_REG_H
#define VEDU_REG_H
/* Define the union U_VEDU_VCPI_INTMASK0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vcpi_enable0_ve_eop  : 1  ;     /* [0] */
        unsigned int    vcpi_enable0_vedu_slice_end : 1  ;     /* [1] */
        unsigned int    vcpi_enable0_ve_buffull : 1  ;     /* [2] */
        unsigned int    vcpi_enable0_ve_pbitsover : 1  ;     /* [3] */
        unsigned int    vcpi_enable0_axi_rdbus_err : 1  ;     /* [4] */
        unsigned int    vcpi_enable0_axi_wrbus_err : 1  ;     /* [5] */
        unsigned int    vcpi_enable0_vedu_timeout : 1  ;     /* [6] */
        unsigned int    reserved_1           : 2  ;     /* [8:7] */
        unsigned int    vcpi_enable0_otp_total_per : 1  ;     /* [9] */
        unsigned int    reserved_0           : 22  ;    /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_vedu_vcpi_intmask0;

#endif
