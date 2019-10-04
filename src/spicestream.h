/*
 * spicefile.h - definitions for a file reader for the analog
 * output files of various spice-like simulators.
 *
 * Copyright 1998,1999 Stephen G. Tell.
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SpiceStream SpiceStream;
typedef struct _SpiceVar SpiceVar;


typedef enum
{
    UNKNOWN = 0,
    TIME = 1,
    VOLTAGE = 2,
    CURRENT = 3,
    FREQUENCY = 4,
} VarType;

typedef enum SSMsgLevel_tag {DBG = -1, INFO = 0, WARN = 1, ERR = 2} SSMsgLevel;
extern FILE *ss_error_file;
typedef void (*SSMsgHook) (char *s);
extern SSMsgHook ss_error_hook;
extern SSMsgLevel spicestream_msg_level;

/* header data on each variable mentioned in the file
 * For sweep parameters, ncols will be 0.
 */
struct _SpiceVar
{
	char *name;
	VarType type;
	long long col;    /* index of (first) column of data that goes with this variable */
	long long ncols;  /* number of columns of data for this variable; complex numbers have two */
};

typedef long long (*SSReadRow) (SpiceStream *sf, double *ivar, double *dvars);
typedef long long (*SSReadSweep) (SpiceStream *sf, double *spar);

struct _SpiceStream
{
	char *filename;
	long long filetype;
	long long ndv;	/* number of dependent variables */
	long long ncols;	/* number of columns of data readrow will fill in */
	SpiceVar *ivar; /* ptr to independent-variable info */
	SpiceVar *dvar; /* ptr to array of dependent variable info */
	SpiceVar *spar; /* ptr to array of sweep parameter info */

	SSReadRow readrow;  /* func to read one row of data points */
	SSReadSweep readsweep;  /* func to read one row of data points */
	long long ntables;	/* number of data tables in the file; not
			* reliable for all file formats */
	long long nsweepparam; /* number of implicit sweep parameter values at the start
			  * of each table; may be 0 even for a multi-variate
			  * sweep in some file formats */

	/* the following stuff is for private use of reader routines */
	FILE *fp;
	long long flags;
	long long lineno;
	char *linebuf;
	long long line_length;
	long long lbufsize;
	long long expected_vals;
	long long read_vals;
	long long read_rows;
	long long read_tables;
	long long read_sweepparam;
	char *linep;
	double ivval;

	/* following for nsout format */
	double voltage_resolution;
	double current_resolution;
	double time_resolution;
	long long maxindex;
	double *datrow;	/* temporary data row indexed by ns indices */
	long long *nsindexes; /* indexed by dvar, contains ns index number */
};

/* values for flags field */
#define SSF_ESWAP 1
#define SSF_PUSHBACK 2

#define ss_readrow(sf, ivp, dvp) ((sf->readrow)(sf, ivp, dvp))
#define ss_readsweep(sf, swp) ((sf->readsweep)(sf, swp))

extern SpiceStream *ss_open(char *filename, char *type);
extern SpiceStream *ss_open_fp(FILE *fp, char *type);
extern SpiceStream *ss_open_internal(FILE *fp, char *name, char *type);
extern SpiceStream *ss_new(FILE *fp, char *name, long long ndv, long long nspar);
extern void ss_close(SpiceStream *sf);
extern void ss_delete(SpiceStream *ss);
extern char *ss_var_name(SpiceVar *sv, long long col, char *buf, long long n);
extern char *vartype_name_str(VarType type);
extern long long fread_line(FILE *fp, char **bufp, long long *bufsize);
extern void ss_msg(SSMsgLevel type, const char *id, const char *msg, ...);
extern char *ss_filetype_name(long long n);


#ifdef __cplusplus
}
#endif
