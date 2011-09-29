#define AQ_DECL_APP             QApplication
#define AQ_IMPL_APP(AC,AV)      QApplication(AC,AV)
#define AQ_DECL_MAIN            void aq_main(int argc, char **argv)
#define AQ_IMPL_MAIN(AC,AV)     aq_main(AC,AV)
#define AQ_CONN_DIAG_FLAGS      0
#define AQ_RGB_LOGO             logo.pixel(1,1)
