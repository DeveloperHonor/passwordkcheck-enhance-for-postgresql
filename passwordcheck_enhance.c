/*-------------------------------------------------------------------------
 *
 * passwordcheck_enchance.c
 *
 * Author: Sungsasong
 *
 * IDENTIFICATION
 *      contrib/passwordcheck_enhance/passwordcheck_enhance.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>

#ifdef USE_CRACKLIB
#include <crack.h>
#endif

#include "commands/user.h"
#include "catalog/namespace.h"
#include "libpq/crypt.h"
#include "fmgr.h"
#include "utils/guc.h"
#if PG_VERSION_NUM < 100000
#include "libpq/md5.h"
#endif

PG_MODULE_MAGIC;

/* passwords shorter than this will be rejected */
#define MIN_PWD_LENGTH 8
#define MIN_UPPER_LETTER  1
#define MIN_LOWER_LETTER  1
#define MIN_DIGIT_CHAR    1
#define MIN_SPECIAL_CHAR  1


extern void _PG_init(void);

/**********************************************************************
 *Function:passwordcheck_enhance                                      *
 *Verifying the password at least need contains one upper letter,lower* 
 *letter,digit and specital character                                 *
 *********************************************************************/

#if PG_VERSION_NUM >= 100000
    static void
    check_password(const char *username,
                   const char *shadow_pass,
                   PasswordType password_type,
                   Datum validuntil_time,
                   bool validuntil_null)
    {
        if (password_type != PASSWORD_TYPE_PLAINTEXT)
        {
            /*
             * Unfortunately we cannot perform exhaustive checks on encrypted
             * passwords - we are restricted to guessing. (Alternatively, we could
             * insist on the password being presented non-encrypted, but that has
             * its own security disadvantages.)
             *
             * We only check for username = password.
             */
            char       *logdetail;

            if (plain_crypt_verify(username, shadow_pass, username, &logdetail) == STATUS_OK)
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("password must not contain user name")));
        }else
        {
            /*
             * For unencrypted passwords we can perform better checks
             */
            const char *password = shadow_pass;
            int            pwdlen = strlen(password);
            int            i;
            // bool        pwd_has_letter,
            //             pwd_has_nonletter;
            int PWD_UPPER_LETTER_COUNT  = 0;
            int PWD_LOWER_LETTER_COUNT  = 0;
            int PWD_SPECIAL_CHAR_COUNT  = 0;
            int PWD_DIGIT_COUNT         = 0;
            int PWD_CONTAINS_LETTER_COUNT = 0;


                //??????????????????8????????????????????????????????????????????????????????????????????????????????????????????????
                for(i = 0; i < pwdlen; i++)
                {
                    /* enforce minimum length */
                    if(pwdlen < MIN_PWD_LENGTH)
                    {
                       ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                 errmsg("???????????????????????? %d ?????????????????????????????????????????????????????????????????? ",MIN_PWD_LENGTH)));
                    }


                    //????????????????????????
                    if(isalpha((unsigned char) password[i]))
                    {
                        PWD_CONTAINS_LETTER_COUNT++;
                        if(islower((unsigned char) password[i]))
                        {
                            PWD_LOWER_LETTER_COUNT++;
                        }else if(isupper((unsigned char) password[i]))
                        {
                            PWD_UPPER_LETTER_COUNT++;
                        }
                    }else if(isdigit((unsigned char) password[i]))
                    {
                        PWD_DIGIT_COUNT++;
                    }else
                    {
                        PWD_SPECIAL_CHAR_COUNT++;
                    }
                }


                //????????????????????????????????????????????????????????????????????????
                if(PWD_LOWER_LETTER_COUNT < MIN_LOWER_LETTER)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_LOWER_LETTER)));
                }else if(PWD_UPPER_LETTER_COUNT < MIN_UPPER_LETTER)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_UPPER_LETTER))); 
                }else if(PWD_DIGIT_COUNT < MIN_DIGIT_CHAR)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ?????????",
                                   MIN_DIGIT_CHAR)));
                }else if(PWD_SPECIAL_CHAR_COUNT < MIN_SPECIAL_CHAR)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_DIGIT_CHAR)));                       
                }



            /* check if the password contains the username */
            if (strstr(password, username))
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("??????????????????????????????")));
            }
        }

        /* all checks passed, password is ok */
    }
#else
    static void
    check_password(const char *username,
                   const char *password,
                   int password_type,
                   Datum validuntil_time,
                   bool validuntil_null)
    {
        int            namelen = strlen(username);
        int            pwdlen = strlen(password);
        char           encrypted[MD5_PASSWD_LEN + 1];
        int            i;
        bool        pwd_has_letter,
                    pwd_has_nonletter;
        int PWD_UPPER_LETTER_COUNT  = 0;
        int PWD_LOWER_LETTER_COUNT  = 0;
        int PWD_SPECIAL_CHAR_COUNT  = 0;
        int PWD_DIGIT_COUNT         = 0;
        int PWD_CONTAINS_LETTER_COUNT = 0;

        switch (password_type)
        {
            case PASSWORD_TYPE_MD5:

                /*
                 * Unfortunately we cannot perform exhaustive checks on encrypted
                 * passwords - we are restricted to guessing. (Alternatively, we
                 * could insist on the password being presented non-encrypted, but
                 * that has its own security disadvantages.)
                 *
                 * We only check for username = password.
                 */
                if (!pg_md5_encrypt(username, username, namelen, encrypted))
                {
                    elog(ERROR, "password encryption failed");
                }
                if (strcmp(password, encrypted) == 0)
                {
                    ereport(ERROR,
                            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                             errmsg("password must not contain user name")));
                }
                break;

            case PASSWORD_TYPE_PLAINTEXT:

                /*
                 * For unencrypted passwords we can perform better checks
                 */

                /* enforce minimum length */
                //??????????????????8????????????????????????????????????????????????????????????????????????????????????????????????
                for(i = 0; i < pwdlen; i++)
                {
                    /* enforce minimum length */
                    if(pwdlen < MIN_PWD_LENGTH)
                    {
                       ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                 errmsg("???????????????????????? %d ?????????????????????????????????????????????????????????????????? ",MIN_PWD_LENGTH)));
                    }


                    //????????????????????????
                    if(isalpha((unsigned char) password[i]))
                    {
                        PWD_CONTAINS_LETTER_COUNT++;
                        if(islower((unsigned char) password[i]))
                        {
                            PWD_LOWER_LETTER_COUNT++;
                        }else if(isupper((unsigned char) password[i]))
                        {
                            PWD_UPPER_LETTER_COUNT++;
                        }
                    }else if(isdigit((unsigned char) password[i]))
                    {
                        PWD_DIGIT_COUNT++;
                    }else
                    {
                        PWD_SPECIAL_CHAR_COUNT++;
                    }
                }


                //????????????????????????????????????????????????????????????????????????
                if(PWD_LOWER_LETTER_COUNT < MIN_LOWER_LETTER)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_LOWER_LETTER)));
                }else if(PWD_UPPER_LETTER_COUNT < MIN_UPPER_LETTER)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_UPPER_LETTER))); 
                }else if(PWD_DIGIT_COUNT < MIN_DIGIT_CHAR)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ?????????",
                                   MIN_DIGIT_CHAR)));
                }else if(PWD_SPECIAL_CHAR_COUNT < MIN_SPECIAL_CHAR)
                {
                    ereport(ERROR,
                           (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("???????????????????????? %d ???????????????",
                                   MIN_DIGIT_CHAR)));                       
                }



            /* check if the password contains the username */
            if (strstr(password, username))
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("??????????????????????????????")));
            }
            
                break;

            default:
                elog(ERROR, "unrecognized password type: %d", password_type);
                break;
        }

        /* all checks passed, password is ok */
    }

#endif

/*
 * Module initialization function
 */

void
_PG_init(void)
{
    /* activate password checks when the module is loaded */
    check_password_hook = check_password;
}
