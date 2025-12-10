/*
 *
 * MIL STD 110A demodulators for use with channel equalizer.
 *
 * Written by C.H Brain G4GUO Aug 2000
 *
 */

#include <math.h>
#include <time.h>
#include "Cm110s.h"
#include "stdio.h"

//#include <direct.h>  //file i/o test data saving not needed when testing done

extern FComplex con_symbol[8];

static unsigned char lastoctet;
static unsigned char firstoctet;
static unsigned char secondoctet;

/*
 *
 * Used in metric calculation software.
 *
 */

void Cm110s::output_rx_bit( int bit )
{


	/*
	if (m_eomreset == 1)
	{
		// Strip off NULL bits
		return;
	}
	else
	{
		// New EOM detection
		m_rxeombuf = (m_rxeombuf << 1) | bit;
		if ((m_rxeombuf ^ 0x4B65A5B2) == 0) m_eomreset = 1;
		//if ((m_rxeombuf ^ 0x4B65A5B2) == 0) new_eom_rx_reset();
		//if ((m_rxeombuf ^ 0x4B65A5B2) == 0) m_rxeombuf = 0;
		//if ((m_rxeombuf ^ 0x4B65A5B2) == 0) return;
	}
	
	//Need code to keep EOM bytes from passing.
	//If all good then buffering 3 octets will do it
*/

	static unsigned char octet;

/*

	if (output_bit_downcount == 0)
	{
		octet = octet << 1;

		if (bit) octet |= 0x01;
		
		output_bit_count++;
		output_octet_count++;
		
		if (output_octet_count == 8)
		{
			firstoctet = octet;
			output_bit_count = 0;
		}
		if (output_octet_count == 16)
		{
			secondoctet = octet;
			output_bit_count = 0;
		}
		if (output_octet_count == 32)
		{
			lastoctet = octet;
			output_bit_count = 0;

		}
		if (output_octet_count == 40)
		{
			output_rx_octet(firstoctet);
			firstoctet = octet;
			output_bit_count = 0;
		}
		if (output_octet_count == 48)
		{
			output_rx_octet(secondoctet);
			secondoctet = octet;
			output_bit_count = 0;
		}
		if (output_octet_count == 56)
		{
			output_rx_octet(lastoctet);
			lastoctet = octet;
			output_bit_count = 0;
			output_octet_count = 40;
		}

	}
	else
	{
		output_bit_downcount--;
	}
*/


	if( output_bit_downcount == 0 )
	{
		octet = octet<<1;

		if( bit ) octet |= 0x01;
		output_bit_count++;
		if( output_bit_count == 8 )
		{
			output_rx_octet( octet );
			output_bit_count = 0;
		}

	}
	else
	{
		output_bit_downcount--;
	}	


	// New EOM detection. Does not work for ASYNC in Local Terminal for some reason
	m_rxeombuf = (m_rxeombuf << 1) | bit;

	// Here scan the entire unsigned integer m_rxeombuf
	//if ((m_rxeombuf ^ 0x4B65A5B2) == 0) m_eomreset = 1;


	// We only check for EOM in 4 LSB bytes so as to not false detect on data 
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5B2) == 0) m_eomreset = 1; return;


	// Damaged first and second bytes so we look for FLUSH NULLs on tail
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5B200) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0xA5B20000) == 0) m_eomreset = 2; return;
	// Damaged B2 EOM tests where last byte is impacted so we look for FLUSH NULL
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5B000) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5B300) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5B600) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5BA00) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5A200) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A59200) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A53200) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5B100) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A5FF00) == 0) m_eomreset = 2; return; // All one's
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x65A50000) == 0) m_eomreset = 2; return; // All zero's
	// Damaged B2 EOM tests where last byte is impacted
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5B0) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5B3) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5B6) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5BA) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5A2) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A592) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A532) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5B1) == 0) m_eomreset = 2; return;
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A5FF) == 0) m_eomreset = 2; return; // All one's
	if ((m_rxeombuf & 0x0000FFFFFFFFFFFFFFFF ^ 0x4B65A500) == 0) m_eomreset = 2; // All zero's


}

void Cm110s::update_mse_average( FComplex error )
{
	float mse;
	
	mse = (float)sqrt((error.re*error.re) + (error.im*error.im));

	mse_average -= mse_magnitudes[mse_count];
	mse_magnitudes[mse_count] = mse;
	mse_average += mse;
	mse_count = ++mse_count%M1_MSE_HISTORY_LENGTH;

}
void Cm110s::update_mse_average_ber(FComplex error)
{
	float mse;

	mse = (float)sqrt((error.re * error.re) + (error.im * error.im));

	mse_average -= mse_magnitudes[mse_count];
	mse_magnitudes[mse_count] = mse;
	mse_average += mse;
	mse_count = ++mse_count % M1_MSE_HISTORY_LENGTH;

	float berval;

	if (viterbi_confidence != 0)
	{

		if (rx_known_errors == 0)
		{
			berval = (1.0 / 10000000.0);
		}
		else
		{
			float vbi_factor;
			vbi_factor = viterbi_confidence/100;
			if (vbi_factor >= 1.0) { vbi_factor = 1.0; }
			berval = ((mse / vbi_factor) / 1920.0);

			//berval = ((mse*(viterbi_confidence/100)) / 1920.0);
			//berval = (mse / 1920.0);
			//berval = ((mse+mse_average/2) / 1920.0);
		}
		func_status(BER_STATUS, &berval);

	}
	else
	{
		berval = (mse / 1920.0);
	
		func_status(BER_STATUS, &berval);
	}
	


}
float Cm110s::get_mse_divisor( void )
{
	float val;
	val = (float)(1.0/(mse_average+0.000001));

	return(val);
}
/*
 *
 * BPSK demodulator 
 *
 */
FDemodulate Cm110s::demodulate_bpsk( FComplex symbol, FComplex rx_scramble )
{
	FDemodulate symd;
	FComplex    sym;
	float       soft;
	float       val;
	float       sd;
	float       max;
	int         max_int;
	int         i;
	DeInterleaver *lvr;

	lvr = &deblk[rx_current_interleaver_load]; 
	
	sym.re  = cmultRealConj(symbol,rx_scramble);
	sym.im  = cmultImagConj(symbol,rx_scramble);


	if(view8psk_state == 0)
	{
		// Here we see the rotation as things fall apart in 2PSK
		// Constellation Symbols
		FComplex    consym;
		consym.re = sym.re *125; //Gain: 50..150
		consym.im = sym.im *125; //Gain: 50..150

		// Constellation Routines
		rx_update_constellation( consym );
	}


	max = 0; 

	for( i = 0; i < 8 ; i+=4 )
	{
		val = cmultRealConj(sym,con_symbol[i]);

		if( val > max )
		{
			max     = val;
			max_int = i;
		}
	}

	soft = max*get_mse_divisor();		

	symd.dx_symbol.re =  con_symbol[max_int].re*0.01f;
	symd.dx_symbol.im =  con_symbol[max_int].im*0.01f;

	// Post Equalizer
	// Constellation Symbols
	FComplex    consym;
	consym.re = symd.dx_symbol.re *125;
	consym.im = symd.dx_symbol.im *125;

	// Constellation Routines
	rx_update_pos_eq_constellation( consym );


	switch( max_int )
	{
		case 0:
			symd.data         =  0;
			sd                =  soft;
			break;
		case 4:
			symd.data         =  1;
			sd                = -soft;
		default:
			break;
	}

	load_deinterleaver( sd, lvr );

	sym.re = cmultReal(symd.dx_symbol,rx_scramble);
	sym.im = cmultImag(symd.dx_symbol,rx_scramble);

	symd.dx_symbol = sym;

	/* Calculate the error */
	symd.error.re = sym.re - symbol.re;  
	symd.error.im = sym.im - symbol.im;  

	/* Save the actual symbol */
	symd.rx_symbol = symbol;

	if(view8psk_state == 1)
	{
		// This provides 8PSK view with movement
		// Constellation Symbols
		FComplex    consym;
		consym.re = symbol.re *125;
		consym.im = symbol.im *125;

		// Constellation Routines
		rx_update_constellation( consym );
	}

	return symd;	

}
/* 
 *
 * QPSK demodulator 
 *
 */
FDemodulate Cm110s::demodulate_qpsk( FComplex symbol, FComplex rx_scramble )
{
	FDemodulate symd;
	FComplex    sym;
	float       val;
	float       soft;
	float       sd[2];
	float       max;
	int         max_int;
	int         i;
	DeInterleaver *lvr;

	lvr = &deblk[rx_current_interleaver_load]; 
	
	sym.re  = cmultRealConj(symbol,rx_scramble);
	sym.im  = cmultImagConj(symbol,rx_scramble);

	if(view8psk_state == 0)
	{
		// Constellation Symbols as QPSK
		FComplex    consym;
		consym.re = sym.re *125; //Gain: 50..150
		consym.im = sym.im *125; //Gain: 50..150

		// Constellation Routines
		rx_update_constellation( consym );
	}

	max = 0; 

	for( i = 0; i < 8 ; i+=2 )
	{
		val = cmultRealConj(sym,con_symbol[i]);

		if( val > max )
		{
			max     = val;
			max_int = i;
		}
	}
	// calculate distance from decision boundry
	soft = max*get_mse_divisor();		

	symd.dx_symbol.re =  con_symbol[max_int].re*0.01f;
	symd.dx_symbol.im =  con_symbol[max_int].im*0.01f;

	// Symbols Post Equalizer
	// Tight 4PSK dislay that doesn't show movement
	// Constellation Symbols
	FComplex    consym;
	consym.re = symd.dx_symbol.re *125;
	consym.im = symd.dx_symbol.im *125;

	// Constellation Routines
	rx_update_pos_eq_constellation( consym );


	switch( max_int )
	{
		case 0:
			symd.data         =  0;
			sd[0]             =  soft;
			sd[1]             =  soft;
			break;
		case 2:
			symd.data         =  1;
			sd[0]             =  soft;
			sd[1]             = -soft;
			break;
		case 4:
			symd.data         =  3;
			sd[0]             = -soft;
			sd[1]             = -soft;
			break;
		case 6:
			symd.data         =  2;
			sd[0]             = -soft;
			sd[1]             =  soft;
			break;
		default:
			break;
	}

	load_deinterleaver( sd[0], lvr );
	load_deinterleaver( sd[1], lvr );

	sym.re = cmultReal(symd.dx_symbol,rx_scramble);
	sym.im = cmultImag(symd.dx_symbol,rx_scramble);

/*
	// Symbols Post Equalizer
	// Nice and tight here, like MIE modem, but 
	// 8PSK vs. 4PSK and doesn't show movement
	// Constellation Symbols
	FComplex    consym;
	consym.re = sym.re *125;
	consym.im = sym.im *125;

	// Constellation Routines
	rx_update_pos_eq_constellation( consym );
*/

	symd.dx_symbol = sym;

	/* Calculate the error */
	symd.error.re = sym.re - symbol.re;  
	symd.error.im = sym.im - symbol.im;  

	/* Save the actual symbol */
	symd.rx_symbol = symbol;


	if(view8psk_state == 1)
	{
		// This provides 8PSK view with movement
		// Constellation Symbols
		FComplex    consym;
		consym.re = symbol.re *125;
		consym.im = symbol.im *125;

		// Constellation Routines
		rx_update_constellation( consym );
	}

	return symd;	
}
/*
 * 
 * 8PSK demodulator 
 *
 */
FDemodulate Cm110s::demodulate_8psk( FComplex symbol, FComplex rx_scramble )
{
	FDemodulate symd;
	FComplex    sym;
	float       val;
	float       max;
	float       soft;
	float       sd[3];
	int         i,max_int;
	DeInterleaver *lvr;

	lvr = &deblk[rx_current_interleaver_load]; 

	sym.re  = cmultRealConj(symbol,rx_scramble);
	sym.im  = cmultImagConj(symbol,rx_scramble);


	// Constellation Symbols
	FComplex    consym;
	consym.re = sym.re *125; //Gain: 50..150
	consym.im = sym.im *125; //Gain: 50..150

	// Constellation Routines
	rx_update_constellation( consym );


	max = 0; 

	for( i = 0; i < 8 ; i++ )
	{
		val = cmultRealConj(sym,con_symbol[i]);

		if( val > max )
		{
			max     = val;
			max_int = i;
		}
	}

	soft = max*get_mse_divisor();		


	symd.dx_symbol.re =  con_symbol[max_int].re*0.01f;
	symd.dx_symbol.im =  con_symbol[max_int].im*0.01f;

	// Post Equalizer
	// Constellation Symbols
//	FComplex    consym;
	consym.re = symd.dx_symbol.re *125;
	consym.im = symd.dx_symbol.im *125;

	// Constellation Routines
	rx_update_pos_eq_constellation( consym );

	switch( max_int )
	{
		case 0:
			symd.data         =  0;
			sd[0]             =  soft;
			sd[1]             =  soft;
			sd[2]             =  soft;
			break;
		case 1:
			symd.data         =  1;
			sd[0]             =  soft;
			sd[1]             =  soft;
			sd[2]             = -soft;
			break;
		case 2:
			symd.data         =  3;
			sd[0]             =  soft;
			sd[1]             = -soft;
			sd[2]             = -soft;
			break;
		case 3:
			symd.data         =  2;
			sd[0]             =  soft;
			sd[1]             = -soft;
			sd[2]             =  soft;
			break;
		case 4:
			symd.data         =  6; 
			sd[0]             = -soft;
			sd[1]             = -soft;
			sd[2]             =  soft;
			break;
		case 5:
			symd.data         =  7;
			sd[0]             = -soft;
			sd[1]             = -soft;
			sd[2]             = -soft;
			break;
		case 6:
			symd.data		  =  5;
			sd[0]             = -soft;
			sd[1]             =  soft;
			sd[2]             = -soft;
			break;
		case 7:
			symd.data      	  =  4;
			sd[0]             = -soft;
			sd[1]             =  soft;
			sd[2]             =  soft;
			break;
		default:
			break;
	}

	if( rx_mode == M4800S )
	{
		output_rx_bit( symd.data&4?1:0 );
		output_rx_bit( symd.data&2?1:0 );
		output_rx_bit( symd.data&1?1:0 );
	}
	else
	{
		load_deinterleaver( sd[0], lvr );
		load_deinterleaver( sd[1], lvr );
		load_deinterleaver( sd[2], lvr );
	}

	sym.re = cmultReal(symd.dx_symbol,rx_scramble);
	sym.im = cmultImag(symd.dx_symbol,rx_scramble);

	symd.dx_symbol = sym;

	/* Calculate the error */
	symd.error.re = sym.re - symbol.re;  
	symd.error.im = sym.im - symbol.im;  

	/* Save the actual symbol */
	symd.rx_symbol = symbol;

	return symd;	
}
//
// 75 bps routines 
//
void Cm110s::reset_sync_75_mask( void )
{
/*
	// Charles Cadet code
	for( int i = 0; i < SYNC_75_MASK_LENGTH; i++ )
	{
		sync_75_mask[i]  = 0.001f;
	}
*/
	// Charles orig code
	for( int i = 0; i < SYNC_75_MASK_LENGTH; i++ )
	{
		sync_75_mask[i]  = m_p_mag[max_p_channel][0][i+frame_start_position];
		sync_75_mask[i] += m_p_mag[max_p_channel][1][i+frame_start_position];
	}

}
void Cm110s::update_sync_75_mask( float *in )
{
	for( int i = 0; i < SYNC_75_MASK_LENGTH; i++ )
	{
		//sync_75_mask[i] = ( sync_75_mask[i]*0.99f ) + ( in[i]*0.01f ); // had been using this

		// The negative here seems to be decode runs on when the signal is trashed and EOM is lost

		// Re-enabled for testing in v1.01 b 1.0.1.3.
		sync_75_mask[i] = ( sync_75_mask[i]*0.50f ) + ( in[i]*0.01f );// Works even better

		//sync_75_mask[i] = ( sync_75_mask[i]*0.70f ) + ( in[i]*0.01f ); // Works better than Charles code

		//sync_75_mask[i] = ( sync_75_mask[i]*0.99f ) + ( in[i]*0.01f ); // Charles had

		// Changing ( in[i]*0.01f ) has no effect.

	}
}
float Cm110s::accumulate_75bps_symbol( FComplex *in, float *out, FComplex *seq )
{
	/*
	FComplex snr_val;
	snr_val = FComplex(*in);
	snr(snr_val);
	*/

	float output;

	output = 0;

	for( int i = 0 ; i < SYNC_75_MASK_LENGTH; i++ )
	{
		out[i]  = match_sequence( &in[i], seq, 32 );
		output += out[i]*sync_75_mask[i];
	}
	return output;
}
void Cm110s::freq_adjust_75bps( FComplex *in )
{
	int i,j,k;
	FComplex temp;
	
	temp.re = 0;
	temp.im = 0;
	
	for( i = 0, k=0; i < 4; i++ )
	{
		for( j = 0; j < 8; j++ )
		{
			temp.re += cmultRealConj( in[k+j], in[k+j+8] );
			temp.im += cmultImagConj( in[k+j], in[k+j+8] );
		}
	}

	temp.re   = (float)(atan2(temp.im,temp.re)*3.97887357729738339422209408431286);
}
void Cm110s::scramble_75bps_sequence( FComplex *in, FComplex *out, int s_count )
{
	for( int i = 0; i < 32; i++ )
	{
		out[i].re = cmultReal( in[i],data_scrambler_seq[(i+s_count)%M1_DATA_SCRAMBLER_LENGTH]);
		out[i].im = cmultImag( in[i],data_scrambler_seq[(i+s_count)%M1_DATA_SCRAMBLER_LENGTH]);
	}

	/*
			// Mostly shows 6o symbols coming across

			// Constellation Symbols
			FComplex    consym;
			consym.re = out->re *600;
			consym.im = out->im *600;

			// Constellation Routines
			rx_update_constellation( consym );
	*/

}
void Cm110s::decode_75bps_data( FComplex *in, int rx_scrambler_count, int is_mes )
{

			// Constellation Symbols
			FComplex    consym;
			consym.re = in->re *100;
			consym.im = in->im *100;

			// Constellation Routines
			rx_update_constellation( consym );


	FComplex temp[4][32];
	//FComplex temp;

	float    tsm[4][SYNC_75_MASK_LENGTH];
	int   i;
	int   data;
	float mag,soft,sum;
	DeInterleaver *lvr;

	lvr = &deblk[rx_current_interleaver_load]; 
		
	// Adjust for any frequency error 
	//freq_adjust_75bps(temp);
	
	soft = 0;
	data = 0;
	sum  = 0;

	// Correlate against all likely sequencies 

	if( is_mes )
	{
		// MES data 

		for( i = 0; i < 4 ; i++ )
		{
			// Produce a scrambled version of what we are looking for
			scramble_75bps_sequence( mes_seq[i], temp[i], rx_scrambler_count );			
			// Produce an output for it
			mag = accumulate_75bps_symbol( in, tsm[i], temp[i] );
			// Find the largest output / most likely symbol
			sum += mag;
			if( mag > soft )
			{
				soft  = mag;
				data  = i; 
			}

			float snr;
			//snr = (float)20.0 * (log10(mag));
			snr = (float)10.0 * (log10(250 * (float)cos(mag) * (mag)));
			//snr = (float) 10.0*(log10(snrmag));
			snr = ((snr + lastsnr) / 2.5f) - 1.5f; //2

			if (isnan(snr) || isinf(snr))
			{
				func_status(SNR_STATUS, &lastsnr);
			}
			else
			{
				func_status(SNR_STATUS, &snr);
			}
			lastsnr = snr;
		}
	}
	else
	{ 
		// MNS data 

		for( i = 0; i < 4 ; i++ )
		{
			// Produce a scrambled version of what we are looking for
			scramble_75bps_sequence( mns_seq[i], temp[i], rx_scrambler_count );			
			// Produce an output for it
			mag = accumulate_75bps_symbol( in, tsm[i], temp[i] );
			// Find the largest output / most likely symbol
			sum += mag;
			if( mag > soft )
			{
				soft  = mag;
				data  = i; 
			}


			float snr;
			//snr = (float)20.0 * (log10(mag));
			snr = (float)10.0 * (log10(250 * (float)cos(mag) * (mag )));
			//snr = (float) 10.0*(log10(snrmag));
			//snr = (snr + lastsnr) / 2;
			snr = ((snr + lastsnr) / 2.5f) - 1.5f; //2
			if (isnan(snr) || isinf(snr))
			{
				func_status(SNR_STATUS, &lastsnr);
			}
			else
			{
				func_status(SNR_STATUS, &snr);
			}
			lastsnr = snr;
		}
	} 
	update_sync_75_mask( tsm[data] );

	soft = (float)sqrt(soft/sum);


	// Undo the modified Gray coding 

	switch( data )
	{
		case 0:
		 	load_deinterleaver(  soft, lvr );
			load_deinterleaver(  soft, lvr );
			break;
		case 1:
		 	load_deinterleaver(  soft, lvr );
			load_deinterleaver( -soft, lvr );
			break;
		case 2:
		 	load_deinterleaver( -soft, lvr );
			load_deinterleaver( -soft, lvr );
			break;
		case 3:
		 	load_deinterleaver(  -soft, lvr );
		 	load_deinterleaver(   soft, lvr );
			break;
		default:
			break;
	}
}
/*
 *
 * Called at the start of each new frame after the preamble has been received.
 *
 */
void Cm110s::demodulate_reset( Mode mode )
{
	viterbi_decode_reset();
	rx_current_interleaver_load  = 0;
	rx_current_interleaver_fetch = 0;
	rx_scrambler_count           = 0;
	output_bit_downcount         = M1_NORMAL_PATH_LENGTH-1;
	output_bit_count             = 0;
    demod_state                  = DEMOD_IDLE_STATE; 
	reset_rx_discard();  // Reset the startup byte discard counter 


	static char ts[50];
	time_t rawtime;
	rawtime = time(NULL);
	struct tm timeinfo;
	gmtime_s(&timeinfo, &rawtime);
	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

	char text[80];
	sprintf_s(text,"%s: Demod IDLE State\r\n",ts);
	func_status(TEXT_STATUS,text);

	switch(mode)
	{
		case M75NS:
			demod                  = &Cm110s::demodulate_bpsk;// Not used
			rx_known_data_length   = 0;
			rx_unknown_data_length = 32; //32; // why isn't this 0 per MIL-STD
			break;
		case M75NL:
			demod                  = &Cm110s::demodulate_bpsk;// Not used
			rx_known_data_length   = 0;
			rx_unknown_data_length = 32; //32; // why isn't this 0 per MIL-STD
			break;
		case M150S:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
			build_known_ib_seq( 7, 4, rx_known_data_length );
			break;
		case M150L:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 5, 4, rx_known_data_length );
			break;
		case M300S:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 6, 7, rx_known_data_length );
			break;
		case M300L:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 4, 7, rx_known_data_length );
			break;
		case M600S:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 6, 6, rx_known_data_length );
			break;
		case M600L:
			demod                  = &Cm110s::demodulate_bpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 4, 6, rx_known_data_length );
			break;
		case M1200S:
			demod                  = &Cm110s::demodulate_qpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 6, 5, rx_known_data_length );
			break;
		case M1200L:
			demod                  = &Cm110s::demodulate_qpsk;
			rx_known_data_length   = 20;
			rx_unknown_data_length = 20;
		    build_known_ib_seq( 4, 5, rx_known_data_length );
			break;
		case M2400S:
			demod                  = &Cm110s::demodulate_8psk;
			rx_known_data_length   = 16;
			rx_unknown_data_length = 32;
		    build_known_ib_seq( 6, 4, rx_known_data_length );
			break;
		case M2400L:
			demod                  = &Cm110s::demodulate_8psk;
			rx_known_data_length   = 16;
			rx_unknown_data_length = 32;
		    build_known_ib_seq( 4, 4, rx_known_data_length );
			break;

		case M600V:
			demod = &Cm110s::demodulate_bpsk;
			rx_known_data_length = 20;
			rx_unknown_data_length = 20;
			build_known_ib_seq(5, 7, rx_known_data_length);

			//WRonG
			//build_known_ib_seq( 5, 7, rx_known_data_length ); // Reserved
			//build_known_ib_seq( 6, 7, rx_known_data_length ); // Reserved

			//build_known_ib_seq( 5, 6, rx_known_data_length ); // Reserved
			//build_known_ib_seq( 5, 7, rx_known_data_length ); // Reserved

			// Reserved, perhaps used for Harris 600bps SYNC MELPe
			// then too the other set for 1200bps SYNC MELPe

			// NOTE: The D1, D2 combinations 5,6 and 5,7 are reserved for 
			// application-specific use. If a demodulator receives any D1, 
			// D2 combination that it does not implement, it shall not 
			// synchronize but shall continue to search for synchronization.

			// Is an Extended Preamble being used?

			break;
		case M1200V:
			demod = &Cm110s::demodulate_qpsk;
			rx_known_data_length = 20;
			rx_unknown_data_length = 20;
			build_known_ib_seq(5, 6, rx_known_data_length);

			//build_known_ib_seq( 5, 6, rx_known_data_length ); // Reserved
			//build_known_ib_seq( 5, 7, rx_known_data_length ); // Reserved

			break;
		case M2400V:
			demod                  = &Cm110s::demodulate_8psk;
			rx_known_data_length   = 16;
			rx_unknown_data_length = 32;
		    build_known_ib_seq( 7, 7, rx_known_data_length );
			break;
		case M4800S:
			demod                  = &Cm110s::demodulate_8psk;
			rx_known_data_length   = 16;
			rx_unknown_data_length = 32;
		    build_known_ib_seq( 7, 6, rx_known_data_length );
			output_bit_downcount   = 0;
			break;
		case MQUERYS:
			rx_known_data_length   = 99;
			break;
		case MQUERYL:
			rx_known_data_length   = 99;
			break;
		default:
			break;
	}
}

/*
 *
 * Viterbi decode and output bitstream.
 *
 */

void Cm110s::demodulate_data_decode( int length )
{
	float metric1;
	float metric2;
	int data;
	int i;
	DeInterleaver *lvr;
	
	lvr = &deblk[rx_current_interleaver_fetch];
	
	switch( rx_mode )
	{
		case M75NS:
		case M75NL:
			for( i = 0; i < length; i++ )
			{
				metric1 = fetch_deinterleaver( lvr );
				metric2 = fetch_deinterleaver( lvr );
				data    = viterbi_decode( metric1, metric2 );
				output_rx_bit( data );
			}
			break;
		case M150S:
		case M150L:
			for( i = 0; i < length; i++ )
			{
				metric1  = fetch_deinterleaver( lvr );
				metric2  = fetch_deinterleaver( lvr );
				metric1 += fetch_deinterleaver( lvr );
				metric2 += fetch_deinterleaver( lvr );
				metric1 += fetch_deinterleaver( lvr );
				metric2 += fetch_deinterleaver( lvr );
				metric1 += fetch_deinterleaver( lvr );
				metric2 += fetch_deinterleaver( lvr );
				data    = viterbi_decode( metric1, metric2 );
				output_rx_bit( data );
			}
			break;
		case M300S:
		case M300L:
			for( i = 0; i < length; i++ )
			{
				metric1  = fetch_deinterleaver( lvr );
				metric2  = fetch_deinterleaver( lvr );
				metric1 += fetch_deinterleaver( lvr );
				metric2 += fetch_deinterleaver( lvr );
				data    = viterbi_decode( metric1, metric2 );
				output_rx_bit( data );
			}
			break;
		case M600S:
		case M600L:
		case M1200S:
		case M1200L:
		case M2400S:
		case M2400L:
		case M600V:
		case M1200V:
		case M2400V:
			for( i = 0; i < length; i++ )
			{
				metric1  = fetch_deinterleaver( lvr );
				metric2  = fetch_deinterleaver( lvr );
				data     = viterbi_decode( metric1, metric2 );
				output_rx_bit( data );
			}
			break;
		case M4800S:
			for( i = 0; i < length; i++ )
			{
				fetch_deinterleaver( lvr );
				fetch_deinterleaver( lvr );
			}
			break;
		case MQUERYS:
			break;
		case MQUERYL:
			break;
		default:
			break;
	}
	if( lvr->nbits == 0 )
	{
		rx_current_interleaver_fetch = (rx_current_interleaver_fetch+1)&1;
		training_dcd_decision();
	}				
}
void Cm110s::demodulate_block( FComplex *in )
{
	int           i,j;
	int           symbol_offset;
	int           votes;
	DeInterleaver *lvr;
	FComplex      known;
	FComplex      sym;

	lvr = &deblk[rx_current_interleaver_load];

	known.re = 1.000f;
	known.im = 0.000f;

	symbol_offset = 0;
	/* Process 150-1200bps */
	if( rx_known_data_length  == 20 )
	{
		for( i = 0; i < 12; i++ )
		{
			for( j = 0; j < 20; j++ )
			{
				equalize_data( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count]);
				rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
				symbol_offset += 2;
			}


// ADDED TEST
equalize_train( in, rx_ib_seq_d1[j] );
equalize_train( in, rx_ib_seq_d2[j] );
//  July 2017 removed from use for testing.
// Keeping seems to help with High Sweep Rate


			// Do the known data 

			rx_block_count++;

			if( rx_block_count == (rx_block_count_mod-1))
			{
				for( j = 0; j < 20; j++ ) //150-600bps
				{
					equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], rx_ib_seq_d1[j] );
	
	switch( rx_mode )
	{
		case M75NS:
			equalize_train( in, rx_ib_seq_d1[j] );
			break;
		case M75NL:
			equalize_train( in, rx_ib_seq_d1[j] );
			break;
		case M150S:
		case M150L:
			equalize_train( in, rx_ib_seq_d1[j] );
			break;
		case M300S:
		case M300L:
			equalize_train( in, rx_ib_seq_d1[j] );
			break;
		case M600S:
		case M600L:
		case M600V:
			equalize_train( in, rx_ib_seq_d1[j] );
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			equalize_train( in, rx_ib_seq_d1[j] ); // ADDED: July 2017 seems to help
			break;
		case M2400S:
		case M2400L:
		case M2400V:
			//equalize_train( in, rx_ib_seq_d1[j] ); // ADDED: July 2017 TEST
			break;
		case M4800S:

			break;
		default:
			break;
	}

					
					
					rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
					symbol_offset += 2;
				}
			}
			else
			{
				if( rx_block_count == rx_block_count_mod )
				{
					for( j = 0; j < 20; j++ ) //1200bps and up
					{
						equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], rx_ib_seq_d2[j] );
						rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
						symbol_offset += 2;
					}
					rx_block_count = 0;
				}
				else
				{
					for( j = 0, votes = 0; j < 20; j++ )
					{
						sym = equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], known );
						rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
						symbol_offset += 2;
						if( sym.re > 0 )
							if( sym.re > fabs( sym.im )) votes++;
					}
					if( votes != 20 )
						rx_known_errors++;
					else
						rx_known_errors = 0;

					// TEST TB#4
					float knownerr;
					knownerr = float(rx_known_errors);
					func_status( BEC_STATUS, &knownerr );

				}
			}
		}
	}
	else
	{
		if( rx_known_data_length == 16 )
		{
			for( i = 0; i < 10; i++ )
			{
				for( j = 0; j < 32; j++ )
				{
					equalize_data( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count]);	
					rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
					symbol_offset += 2;	
				}	

				// Do the known data 

				rx_block_count++;

				if( rx_block_count == (rx_block_count_mod-1))
				{
					for( j = 0; j < 16; j++ )
					{
						equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], rx_ib_seq_d1[j] );
						rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
						symbol_offset += 2;	
					}
				}
				else
				{
					if( rx_block_count == rx_block_count_mod )
					{
						for( j = 0; j < 16; j++ )
						{
							equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], rx_ib_seq_d2[j] );
							rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
							symbol_offset += 2;	
						}
						rx_block_count = 0;
					}
					else
					{
						for( j = 0, votes = 0; j < 16; j++ )
						{
							sym = equalize_train_seq( &in[symbol_offset], data_scrambler_seq[rx_scrambler_count], known );
							rx_scrambler_count = ++rx_scrambler_count%M1_DATA_SCRAMBLER_LENGTH;
							symbol_offset += 2;	
							if( sym.re > 0 ) 
								if( sym.re > fabs( sym.im )) votes++;
						}
						if( votes != 16 )
							rx_known_errors++;
						else
							rx_known_errors = 0;

					// TEST TB#4
					float knownerr;
					knownerr = float(rx_known_errors);
					func_status( BEC_STATUS, &knownerr );

					}
				}
			}
		}
		else
		{
			// 75 BPS

			if( rx_known_data_length == 0 )
			{
				for( i = 0; i < 15; i++ )
				{
					rx_block_count++;

					if( rx_block_count == rx_block_count_mod )
					{
						/* Exception set */
						decode_75bps_data( &in[symbol_offset], rx_scrambler_count, 1 );
						rx_scrambler_count = (rx_scrambler_count+32)%M1_DATA_SCRAMBLER_LENGTH;
						symbol_offset += 64;
						rx_block_count = 0;

					}
					else
					{	 
						/* Normal set */
						decode_75bps_data( &in[symbol_offset], rx_scrambler_count, 0 );
						rx_scrambler_count = (rx_scrambler_count+32)%M1_DATA_SCRAMBLER_LENGTH;
						symbol_offset += 64;
					}
				}
			}
		}
	}


	kalman_reset_ud();

	//Output the demodulated Data Byte
	if( demod_state == DEMOD_RX_STATE ) demodulate_data_decode( number_of_bits_in_200ms( rx_mode ));
	//state_text();


    if( demod_state  == DEMOD_IDLE_STATE )
	{

		if( lvr->nbits == lvr->size ) 
		{
			// We fall through here only once per incoming transmission
			rx_current_interleaver_load  = 1;
			rx_current_interleaver_fetch = 0;
			demod_state = DEMOD_RX_STATE;

			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];
			sprintf_s(text,"%s: Demod RX State\r\n",ts);
			func_status(TEXT_STATUS,text);

			//Hardware Modem ASYNC Decoder Force Reset
			m_eomreset = 0; // new eom reset
			float async_decoder_reset;
			async_decoder_reset = 5555.0;
			func_status(TEXT_STATUS, &async_decoder_reset);

			/*
			//Hardware modem ASYNC Decoder starting point force
			unsigned char (decoderstart);
			decoderstart = 0xFF; // MARK byte to FORCE decoder state
			output_rx_octet( decoderstart );
			output_bit_count = 0;
			*/




			//state_text();
		}
	}
	else
	{

		/*
			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];


			//Note: these should be enabled/disabled by MMI commands
			//but only one at a time vs. both

			// need this to print when no other Status window output
			func_status(TEXT_STATUS,"");
			//
			sprintf_s(text,"RX Block %i",rx_block_count);
			// Send to MS-DMT State Window
			sprintf_s( m_state_text,text);

			sprintf_s(text,"%s: RX Block Count = %i\r\n",ts,rx_block_count);
			// Send to MS-DMT Status Window
			func_status(TEXT_STATUS,text);

		*/


		if( lvr->nbits == lvr->size ) 
		{
			rx_current_interleaver_load = (rx_current_interleaver_load+1)&1;	

		}

	}
	
	// RX_KNOWN_ERROR_THRESHOLD set to 5000
	//if( rx_known_errors > RX_KNOWN_ERROR_THRESHOLD ) rx_reset();

/*
	if( rx_known_errors>0)
	{
		FILE *out_fp; // File pointer of logging file
		static char outFileName[255];
		static char *CurrentPath;
		static char *outlogSubDirectory;
		int curdrive;

		outlogSubDirectory = "\\outlog\\";

		// Fix to allow for support on any system drive
		curdrive = _getdrive();
		CurrentPath = _getdcwd( curdrive, NULL, 0 );

		strcpy(outFileName,CurrentPath);
		strcat(outFileName,outlogSubDirectory);
		strcat(outFileName,"rx_known_errors_test");
		strcat(outFileName,".out");

		//MessageBox (NULL, outFileName, "test.out Path", MB_OK);

		out_fp = fopen(outFileName,"a");

		if(out_fp != NULL)
		{
			//fputs(LPCTSTR(out),out_fp);
			fprintf (out_fp,"%i\n",rx_known_errors);

		}
		fclose(out_fp);
	}
*/

}
