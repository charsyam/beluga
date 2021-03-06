/* -Wv --std=c90 --input-charset=big5 */
/* m_36_big5.c: Handling of '\\' in BigFive multi-byte character.    */

#define     str( a)     # a

main( void)
{
    fputs( "started\n", stdout);

/* 36.1:    0x5c in multi-byte character is not an escape character */

#pragma __setlocale( "big5")                /* For MCPP     */
#pragma setlocale( "chinese-traditional")   /* For Visual C */

#if     '字' == '\xa6\x72' && '功' != '\xa5\x5c'
    fputs( "Bad handling of '\\' in multi-byte character", stdout);
    exit( 1);
#endif

/* 36.2:    # operater should not insert '\\' before 0x5c in multi-byte
        character   */
    assert( strcmp( str( "功績"), "\"功績\"") == 0);
    fputs( "功績" "\"功績\"\n", stdout);

    fputs( "success\n", stdout);
    return  0;
}

