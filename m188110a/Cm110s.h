/*
 * General header file for MIL-110A.
 *
 */

#ifndef CM110S_H
#define CM110S_H

#include "platform_compat.h"
#include <queue>

//using namespace std;

typedef struct{
    float re;
    float im;
}FComplex;
/*
typedef enum{
    M75NN   = 0,
    M75NS   = 1,
    M75NL   = 2,
    M150N   = 3,
    M150S   = 4,
    M150L   = 5,
    M300N   = 6,
    M300S   = 7,
    M300L   = 8,
    M600N   = 9,
    M600S   = 10,
    M600L   = 11,
    M1200N  = 12,
    M1200S  = 13,
    M1200L  = 14,
    M2400N  = 15,
    M2400S  = 16,
    M2400L  = 17,
    M2400V  = 18,
    M4800S  = 19,
    MQUERYS = 20,
    MQUERYL = 21,
    MMAX
}Mode;
*/

typedef enum{
    M75NS   = 0,
    M75NL   = 1,
    M150S   = 2,
    M150L   = 3,
    M300S   = 4,
    M300L   = 5,
    M600S   = 6,
    M600L   = 7,
    M1200S  = 8,
    M1200L  = 9,
    M2400S  = 10,
    M2400L  = 11,
    M600V   = 12,
    M0000N =  13,
    M1200V  = 14,
    M0001N  = 15,
    M2400V  = 16,
    M4800S  = 17,
    MQUERYS = 18,
    MQUERYL = 19,
    SEND_FILE = 20, // Winsock
    MMAX
}Mode;


typedef struct {
    int      data;
    FComplex rx_symbol;
    FComplex dx_symbol;
    FComplex error;
}FDemodulate;

typedef enum{
    DCD_TRUE_STATUS,
    DCD_FALSE_STATUS,
    TX_TRUE_STATUS,
    TX_FALSE_STATUS,
    FREQ_ERROR_STATUS,
    //TEXT_STATUS,
    SNR_STATUS,
    PREAM_SNR_STATUS,
    VITERBI_STATUS,
    TRAIN_STATUS,
    TEXT_STATUS,
    BER_STATUS,
    BEC_STATUS
}ModemStatus;

typedef unsigned short U16;
typedef unsigned char  U8;

/*
 * row      == row counter
 * col      == col counter
 * row_inc  == row increment
 * col_inc  == col increment
 * row_nr   == number of rows
 * col_nr   == number of columns
 * col_last == value of col counter when last row counter was 0
 * size     == total size in the array
 * nbits    == number of bits used in the array.
 * mode     == what data mode it is set for.
 * zrow     == zero interleaver output row counter.
 * zcol     == zero interleaver output column counter.
 *
 */

typedef struct{
    float array[40][576];
    int  row;
    int  row_inc;
    int  row_nr;
    int  col;
    int  col_inc;
    int  col_nr;
    int  col_last;
    int  size;
    int  nbits;
    Mode mode;
    int  zrow;
    int  zcol;
}DeInterleaver;

typedef struct{
    int  array[40][576];
    int  row;
    int  row_inc;
    int  row_nr;
    int  col;
    int  col_inc;
    int  col_nr;
    int  col_last;
    int  size;
    int  nbits;
    Mode mode;
    int  zrow;
    int  zcol;
}Interleaver;

typedef struct{
    int bit1;
    int bit2;
}ParityLookup;

// Viterbi
#define M1_NR_CONVOLUTIONAL_STATES     64
#define M1_NORMAL_PATH_LENGTH          144

// 75 BPS
//#define M1_FINE_SYNC_UPDATE_THRESHOLD  500

/* AGC constant */
//#define M1_LAMBDA                      0.02 // Charles

//BIG GIRL
#define M1_LAMBDA                      0.08 // Works good
//#define M1_LAMBDA                      0.07


#define M1_DATA_SCRAMBLER_LENGTH       160
//
// Equaliser constants.
//
#define M1_FF_EQ_LENGTH           32
#define M1_FB_EQ_LENGTH           8
#define M1_KN                     (M1_FF_EQ_LENGTH+M1_FB_EQ_LENGTH)
#define M1_TX_BIT_ARRAY_LENGTH    400000



#ifndef PI
#define PI                        3.1415926535897932384
#endif

#define M1_P_COMMON_LENGTH        288
#define M1_P_MODE_LENGTH          64
#define M1_P_COUNT_LENGTH         96
#define M1_P_ZERO_LENGTH          32

#define M1_TX_FILTER_LENGTH       32
#define M1_RX_FILTER_LENGTH 	  19
#define M1_TX_BLOCK_LENGTH        1920
#define M1_RX_BLOCK_LENGTH        1920
#define M1_TX_ARRAY_LENGTH        M1_TX_BLOCK_LENGTH*30

#define M1_SAMPLE_RATE            9600 // Modem sample rate clock
#define M1_CENTER_FREQUENCY       1800 // MIL-STD PSK Carrier
#define M2_CENTER_FREQUENCY       1650 // Alternate PSK Carrier
#define M3_CENTER_FREQUENCY       1500 // Alternate PSK Carrier

#define M1_ON_FREQ                0
#define M1_HI_FREQ                1
#define M1_LO_FREQ                2

// MSE = Mean Square Error
// Charles had
//#define M1_MSE_HISTORY_LENGTH     256

// ADDED TEST
//#define M1_MSE_HISTORY_LENGTH     128

#define M1_MSE_HISTORY_LENGTH     192

// Loss of DCD threshold to reset receiver
// If missing EOM the errors will rise. However
// you can have high bit errors into the thousands
// from multipath
#define RX_KNOWN_ERROR_THRESHOLD 5000 //100 per Charles

// 75 bps decoder
#define SYNC_75_MASK_LENGTH 32

// Chanel Identities
#define LO_CHANNEL 0
#define ON_CHANNEL 1
#define HI_CHANNEL 2

/* Complex math operation */

#define cmultReal(x,y)     (x.re*y.re)-(x.im*y.im)
#define cmultImag(x,y)     (x.re*y.im)+(x.im*y.re)
#define cmultRealConj(x,y) (x.re*y.re)+(x.im*y.im)
#define cmultImagConj(x,y) (x.im*y.re)-(x.re*y.im)



//
// Serial Modem Class
//

class Cm110s
{
    public:
    Cm110s();
    ~Cm110s();

    void test();

    //
    // Constellation read routines
    //
    int ReadConstellation( FComplex *in, int length );

    int ReadConstellationPeq( FComplex *in, int length ); // Post Equalization

    // EOM reset
    int	m_eomreset;

    //
    // Mode setting routines.
    //
    void         tx_enable(void){m_tx_enable = true;}
    void         rx_enable(void){m_rx_enable = true;}
    void         tx_disable(void){m_tx_enable = false;}
    void         rx_disable(void){m_rx_enable = false;}
    void         rx_reset( void );
    void         new_eom_rx_reset(void); // Called by new EOM bit detector in de110a.cpp
    void         eom_rx_reset( void ); // Called by EOM validation routines in main program
    void         post_eom_rx_reset( void ); // Called by EOM Null Byte Count validation routines in main program
    void         tx_set_mode( Mode mode );
    void         tx_set_mode( const char * );
    //
    // Transmit data routines
    //
    void         tx_frame( U8 *data, int length, Mode mode );
    void         tx_frame( U8 *data, int length );
    void         tx_sync_frame( U8 *data, int length );
    void         tx_sync_frame_eom( U8 *data, int length );

    //ADDED Test functions in attempt to send while buffering data port in DMT
    void         tx_data_port_sync_frame( unsigned char *data, int length );
    void         tx_data_port_flush( int length );


    void		 tx_bit( int bit );
    //
    // Informational routines.
    //
    const char*  rx_get_mode_string( void );
    const char*  tx_get_mode_string( void );
    Mode         tx_get_mode( void );
    Mode         rx_get_mode( void );
    Mode         tx_get_decremented_bitrate( void );
    Mode         tx_get_incremented_bitrate( void );
    Mode         tx_get_toggled_interleave( void );
    bool         rx_is_receiving( void );
    char         m_state_text[2048];
    void         state_text( void );
    //void         get_state_text( char *resp );
    const char*  get_state_text( );

    //
    // Configuration routines.
    //
    void         tx_set_soundblock_size( int soundblock_size );
//	void         rx_callback( U16 *in, int length );
    void         register_receive_octet_callback_function(void(*callback)(U8));
    void         register_status(void(*status)(ModemStatus,void*));
    //
    // Soundcard Interface routines.
    //
    void         rx_process_block( signed short *in, int length );
    float        *tx_get_soundblock( void );
    void         tx_release_soundblock( float *block );
    void         set_psk_carrier( long carrier );
    //void         tx_psk_carrier( long carrier );

    void         set_preamble_hunt_squelch( float squelch );

    void         set_noise_reduction_state( int nr ); // 0 = OFF, 1 = ON
    void         set_agc_state( int agc ); // 0 = OFF, 1 = ON
    int          get_agc_state();
    void         set_view8psk( int view8psk ); // 0 = OFF, 1 = ON

    //P-Mode
    void         set_p_mode( int pmode ); // 0 = OFF, 1 = ON
    //E-Mode
    void         set_e_mode( int emode ); // 0 = OFF, 1 = ON
    //B-Mode
    void         set_b_mode( int bmode ); // 0 = OFF, 1 = ON
    //K-Mode
    void         set_k_mode( int kmode ); // 0 = OFF, 1 = ON
private:

	unsigned int  m_rxeombuf;
	//int           m_eomreset;

	typedef enum{TX_IDLE_STATE,TX_PREAMBLE_STATE,TX_DATA_STATE,TX_CLOSING_STATE}TxState;
	typedef enum{RX_HUNTING_PREAMBLE_STATE,RX_PREAMBLE_FOUND_STATE,RX_DATA_RECEIVING_STATE}RxState;
	typedef enum{DEMOD_IDLE_STATE,DEMOD_RX_STATE}DemodState;
	bool         m_tx_enable;
	bool         m_rx_enable;
	FComplex     mns_seq[4][32];
	FComplex     mes_seq[4][32];
	FComplex     prx[8][32];// symbols used in preamble
	int          rx_scrambler_count;
	int          rx_unknown_data_length;
	int          rx_known_data_length;
	int          rx_known_errors;
	FDemodulate  (Cm110s::*demod)(FComplex,FComplex);
	void         (*rx_callbk)(unsigned char);
	void         (*func_status)(ModemStatus,void*);
	int          rx_current_interleaver_load;
	int          rx_current_interleaver_fetch;
	FComplex     rx_ib_seq_d1[20];
	FComplex     rx_ib_seq_d2[20];
	int          rx_block_count;
	int          rx_block_count_mod;
	float        rx_block_buffer[M1_RX_BLOCK_LENGTH];
	int          rx_block_buffer_index;
	int          ptt_active_count;
	float        sync_75_mask[SYNC_75_MASK_LENGTH];
//
// AGC
	//double       m_hold;
	float       m_hold;

// SNR

	float preamsnr;
	float datasnr;
	float lastsnr;
	float lasthuntsnr;
	float lastpreamblesnr;
	float lastdatasnr;

	float lastderror;

//
// Variables used in equaliser
//
	FComplex     d_eq[M1_KN];
	FComplex     c[M1_KN];
	FComplex     f[M1_KN];
	FComplex     g[M1_KN];
	FComplex     u[M1_KN][M1_KN];
	FComplex     h[M1_KN];
	float        d[M1_KN];
	float        a[M1_KN];
	float        y;
	float        q;
	float        E;
//
	int          tx_bit_array[M1_TX_BIT_ARRAY_LENGTH];
	int          in_ptr;
	int          out_ptr;
//
// Preamble Sequences
//
	FComplex     p_common[M1_P_COMMON_LENGTH];
	FComplex     p_mode[MMAX][M1_P_MODE_LENGTH];
	FComplex     p_count[24][M1_P_COUNT_LENGTH];
	FComplex     p_zero[M1_P_ZERO_LENGTH];
//
	RxState      rx_state;
	Mode         rx_mode=M300L;										//BB October 2022.  Initialized here.  Removed assignments from rx_reset functions toenable MATCHDR to work.
	FComplex     b1[M1_RX_BLOCK_LENGTH*2];
	FComplex     b2[M1_RX_BLOCK_LENGTH+(M1_RX_BLOCK_LENGTH/2)];
	int          frame_start_position;
	// Mags of last frame and current preamble
	// Note: array sized to account for index offset of M1_FF_EQ_LENGTH/2 in rxm110a.cpp
	float        m_p_mag[3][2][M1_RX_BLOCK_LENGTH/2 + M1_FF_EQ_LENGTH/2];
	int          max_p_channel;
//
	FComplex     tx_preamble[11520];
	int          tx_preamble_length;
	int          tx_known_data_length;
	int          tx_unknown_data_length;
	int          tx_block_count;
	int          tx_symbol_count;
	int          tx_block_count_mod;
	int          tx_ib_seq_d1[20];
	int          tx_ib_seq_d2[20];
	Mode         tx_mode;
	int          data_scrambler_bits[M1_DATA_SCRAMBLER_LENGTH];
	FComplex     data_scrambler_seq[M1_DATA_SCRAMBLER_LENGTH];
	FComplex     tx_filter_buffer[M1_TX_FILTER_LENGTH/4];
	int          tx_data_scrambler_offset;
	int          tx_preamble_count;
//
	TxState      tx_state;
	int          tx_current_interleaver_load;
	int          tx_current_interleaver_fetch;
//
	Interleaver   inblk[2];
	DeInterleaver deblk[2];
//
	ParityLookup parity_lookup[128];
	int          encode_state;
	float        acm[M1_NR_CONVOLUTIONAL_STATES]; /* Accumulated distance */
	int          path[M1_NR_CONVOLUTIONAL_STATES][M1_NORMAL_PATH_LENGTH];
	int          hp;
	int          path_length;
//
	float        mse_magnitudes[M1_MSE_HISTORY_LENGTH];
	float        mse_average;
	int          mse_count;
	DemodState   demod_state;
	int          output_bit_downcount;
	int          output_bit_count;

	int          output_octet_downcount;
	int          output_octet_count;

//
//  Transmit soundblock queue.
//
    std::queue<float*>tx_sb_q;
	float       *current_tx_soundblock;
	int          current_sound_block_pointer;
	int          tx_soundblock_size;

	//
// PSK Carrier Select - Supports 1500, 1650 and 1800hz
//
	long          psk_carrier_select;




// Constellation routines
// Contellation queue

    std::queue<FComplex>m_c_q;
    std::queue<FComplex>m_c_q_peq;  // Post Equalizer

// PMAG Squelch factor

	float         pmag_sql_factor;
	float         pmag_multiplier;

// Noise Reduction

	int         nr_state;

// AGC

	int         agc_state;

// False Detects factor

	float         false_detect_factor;

// Constellation 8PSK View

	int         view8psk_state;

// Constellation Display Modes

	int         pmode_state;  // RAW Pre-Equalizer Symbols
	int         emode_state;  // Equalizer output symbols
	int         bmode_state;  // Both P and E
	int         kmode_state;  // KNOWN or UNKNOWN Equalizer output symbol selection

//
// DCD decision
//
	int m_train_errors;
	int m_train_count;

	float        viterbi_confidence;
	int		     viterbi_state;

//
// Functions.
//
	void         demodulate_block( FComplex *in );
	void         demodulate_reset( Mode mode );
	void         output_rx_octet( U8 octet );
	void         reset_rx_discard();
	void         update_mse_average( FComplex error );
	void         update_mse_average_ber(FComplex error);
	const char  *display_mode_string( Mode mode );
//
	void		 parity( int state, int *bit1, int *bit2 );
	void         viterbi_init( void );
	void         viterbi_encode_reset( void );
	void         viterbi_decode_reset( void );
	void         viterbi_encode( int in, int *bit1, int *bit2 );
	int          viterbi_decode( float metric1, float metric2 );
//
	void         kalman_reset_coffs(void);
	void 	     kalman_reset_ud( void );
	void         kalman_reset( void );
	void		 kalman_init( void );
	void         kalman_calculate( FComplex *x );
	void         kalman_update( FComplex *data, FComplex error );
	void         equalize_init( void );
	void         equalize_reset( void );
	void		 update_fb( FComplex in );
	void         equalize_update_initial_fb( FComplex *train, int count );
	FComplex     equalize( FComplex *in );
	FComplex     equalize_train( FComplex *in, FComplex train );
	FComplex     equalize_train_seq( FComplex *in, FComplex scramble, FComplex train );
	FComplex     equalize_rate_train( FComplex *in, FComplex train, FComplex rate );
	FDemodulate  equalize_data( FComplex *in, FComplex scrambler );
	void         save_fb_data( void );
	void 	     restore_fb_data( void );
	void         train_on_preamble( FComplex *in, Mode mode, int count );
//
	int		     calculate_interleaver_size( Interleaver *lvr );
	int          calculate_deinterleaver_size( DeInterleaver *lvr );
	void         reset_interleave( Interleaver *lvr );
	void         load_interleaver( int bit, Interleaver *lvr );
	void         clear_deinterleave( DeInterleaver *lvr );
	int          fetch_interleaver( Interleaver *lvr );
	void         load_deinterleaver( float bit, DeInterleaver *lvr );
	float        fetch_deinterleaver( DeInterleaver *lvr );
	void         reset_deinterleave( DeInterleaver *lvr );
	void         set_interleave_mode( Mode mode, Interleaver *lvr );
	void         set_de_interleave_mode( Mode mode, DeInterleaver *lvr );
	int          get_number_of_bits_in_interleaver( Mode mode );
//
	int          create_preamble_and_known_data( Mode mode, FComplex *buff );
	void         init_preamble_module( void );
	void         build_known_ib_seq( int d1, int d2, int length );
	void		 create_prx_tables( void );
	int          fcomplex_copy( FComplex *dest, FComplex *src, int length );
	void         set_known_inter_block_seq( int d1, int d2 );
	int          add_count_seq( int count, int *buff );
	int          add_common_preamble_seq( int *buff );
	int          create_common_preamble_sequence( FComplex *buff );
	int          create_d_preamble_sequence( int d, FComplex *buff );
	int          create_count_preamble_sequence( int count, FComplex *buff );
	int          create_zero_preamble_sequence( FComplex *buff );
	int          create_short_preamble( Mode mode, FComplex *buff );
	int          create_long_preamble( Mode mode, FComplex *buff );
//
// RXM
//
	bool		 verify_mode( Mode mode, int count );
	float        match_sequence( FComplex *in, FComplex *seq, int length );
	void         set_rx_mode( Mode mode );
	void         reset_frame_start( int val );
	int          get_frame_start( void );
	void         remove_frequency_error( FComplex *in, float freq, int length );
	void         translate_seq_in_freq( FComplex *in, FComplex *out, float freq, int length );
	FComplex     agc( FComplex in  );
	FComplex     agc_2( FComplex in  );
	FComplex     snr( FComplex in  );
	FComplex     filter( FComplex *in, const float *coffs, int length );
	Mode         find_mode_value( FComplex *in );
	int          find_count_value( FComplex *in );
	int          extract_preamble_symbol( FComplex *in );
	bool         verify_preamble( FComplex *in );
	float        correlate_common_preamble( FComplex *in );
	float        calculate_frequency_error( FComplex *in );

	int          find_frequency_error_and_start_of_preamble( FComplex *in, float &error, float &pmag, Mode &mode, int &count );
	
	int          new_start_find_frequency_error_and_start_of_preamble( FComplex *in, float &error, float &pmag, Mode &mode, int &count );
	
	int          display_frequency_error_hunting_start_of_preamble( FComplex *in, float &error, float &pmag, Mode &mode, int &count );
	void         process_receive_block( float *in );
//
	void         create_data_scrambler_seq( void );
	void         create_mes_and_mns_seq( void );
//
	U16          *get_tx_block( void );
	int          number_of_bits_in_200ms( Mode mode );
	int          *get_bits_to_send( int length );
//
	void         output_rx_bit( int bit );
	float        get_mse_divisor( void );
	FDemodulate  demodulate_bpsk( FComplex symbol, FComplex rx_scramble );
	FDemodulate  demodulate_qpsk( FComplex symbol, FComplex rx_scramble );
	FDemodulate  demodulate_8psk( FComplex symbol, FComplex rx_scramble );

	void         reset_sync_75_mask( void );
	void         update_sync_75_mask( float *in );
	void		 scramble_75bps_sequence( FComplex *in, FComplex *out, int s_count );
	float        accumulate_75bps_symbol( FComplex *in, float *mask, FComplex *seq );
	void         freq_adjust_75bps( FComplex *in );
	void         decode_75bps_data( FComplex *in, int rx_scrambler_count, int is_mes );
	void         demodulate_data_decode( int length );
	void         rx_known_symbols( void );
	int          octet_to_bit_arrary( U8 *octets, int *bits, int octet_length );
	void         send_octet_array( U8 *octet, int length );
	void         send_sync_octet_array( U8 *octet, int length );
//
	float        twos_to_float( U16 in );
	void         tx_symbol( FComplex symbol );
	void		 tx_silence( void );
	void         transmit_load_interleaver_block( int *bits, int length );
	void		 transmit_preamble_reset( void );
	void         transmit_preamble( void );
	void         transmit_unknown_data_symbol( int sym );
	void		 tx_0_known_inter_block_data_symbols( void );
	void         tx_d1_known_inter_block_data_symbols( void );
	void         tx_d2_known_inter_block_data_symbols( void );
	void		 tx_known_symbols( void );
	int          transmit_interleaver_block( int length );
	void         transmit_load_interleaver( int *bits, int length );
	void         prepare_fresh_tx_samples( void );
	// DCD routines
	void         training_dcd_decision( void );
	void         training_dcd_reset( void );



	// Constellation routines

	void rx_update_constellation( FComplex sym );

	void rx_update_pos_eq_constellation( FComplex sym );




};

#endif
