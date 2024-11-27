
/*
 * system.c
 */

/**
this is only sample code. Please do change own your resposiblity.
 */

/**
 * Returns the system uptime in centiseconds.
 *
 * @note The value returned by this function is not identical to sysUpTime
 *   defined in RFC 1213. get_uptime() returns the system uptime while
 *   sysUpTime represents the time that has elapsed since the most recent
 *   restart of the network manager (snmpd).
 *
 * @see See also netsnmp_get_agent_uptime().
 */

/* Changed Here - renamed function name to get_uptime_old from get_uptime */
long
get_uptime_old(void)  
{
#if defined(aix4) || defined(aix5) || defined(aix6) || defined(aix7)
    struct nlist nl;
    int kmem;
    time_t lbolt;
    nl.n_name = "lbolt";
    if(knlist(&nl, 1, sizeof(struct nlist)) != 0) return(0);
    if(nl.n_type == 0 || nl.n_value == 0) return(0);
    if((kmem = open("/dev/mem", 0)) < 0) return 0;
    lseek(kmem, (long) nl.n_value, L_SET);
    read(kmem, &lbolt, sizeof(lbolt));
    close(kmem);
    return(lbolt);
#elif defined(solaris2)
    kstat_ctl_t    *ksc = kstat_open();
    kstat_t        *ks;
    kid_t           kid;
    kstat_named_t  *named;
    u_long          lbolt = 0;

    if (ksc) {
        ks = kstat_lookup(ksc, "unix", -1, "system_misc");
        if (ks) {
            kid = kstat_read(ksc, ks, NULL);
            if (kid != -1) {
                named = kstat_data_lookup(ks, "lbolt");
                if (named) {
#ifdef KSTAT_DATA_UINT32
                    lbolt = named->value.ui32;
#else
                    lbolt = named->value.ul;
#endif
                }
            }
        }
        kstat_close(ksc);
    }
    return lbolt;
#elif defined(linux) || defined(cygwin)
    FILE           *in = fopen("/proc/uptime", "r");
    long            uptim = 0, a, b;
    if (in) {
        if (2 == fscanf(in, "%ld.%ld", &a, &b))
            uptim = a * 100 + b;
        fclose(in);
    }
    return uptim;
#else
    struct timeval  now;
    long            boottime_csecs, nowtime_csecs;

    boottime_csecs = get_boottime();
    if (boottime_csecs == 0)
        return 0;
    gettimeofday(&now, (struct timezone *) 0);
    nowtime_csecs = (now.tv_sec * 100) + (now.tv_usec / 10000);

    return (nowtime_csecs - boottime_csecs);
#endif
}

/* Changed Here - introduce new function get_uptime */
long
get_uptime(void){
 return get_uptime_old()/100;
}
