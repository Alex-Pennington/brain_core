/*
 *
 * Transmit module.
 *
 */
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include "Cm110s.h"
// Qt removed: #include <QThread>

extern float tx_coffs[M1_TX_FILTER_LENGTH];
extern FComplex con_symbol[8];
extern int mgd3[8];
extern int mgd2[4];
extern int mns[4][32];
extern int mes[4][32];
extern int psk8[8]; 
extern int psk4[4]; 
extern int psk2[8]; 

/* Set the transmit mode */

void Cm110s::tx_set_mode( Mode mode )
{
	if( tx_state == TX_IDLE_STATE )
	{
		tx_mode = mode;
		tx_preamble_length = create_preamble_and_known_data( tx_mode, tx_preamble );
		set_interleave_mode( tx_mode, &inblk[0] );
		set_interleave_mode( tx_mode, &inblk[1] );
	
		// Why is this needed? If not remarked out causes CAT PTT OFF and
		// any CAT Data/Voice off commands to be sent.
		
		//func_status( TX_FALSE_STATUS, NULL );
	}
}

Mode Cm110s::tx_get_mode( void )
{
	return( tx_mode );
}

/*
 *
 * Transmit a channel symbol.
 *
 */
void Cm110s::tx_symbol( FComplex symbol )
{
	int             i,k;
	static float    acc;
	FComplex        output;
	float           sample;
	
	/* Update the filter input buffer */
 	
	for( i = 0; i < (M1_TX_FILTER_LENGTH/4)-1; i++)
	{
		tx_filter_buffer[i] = tx_filter_buffer[i+1];
	}
	tx_filter_buffer[i] = symbol;

	/* Do the filtering and produce 4 output samples */

	for( k = 0; k < 4; k++ )
	{
		output.re = tx_filter_buffer[0].re*tx_coffs[k];
		output.im = tx_filter_buffer[0].im*tx_coffs[k];
		
		for( i = 1; i < (M1_TX_FILTER_LENGTH/4); i++)
		{
			output.re += tx_filter_buffer[i].re*tx_coffs[(i*4)+k];
			output.im += tx_filter_buffer[i].im*tx_coffs[(i*4)+k];
		}
		/* Up convert the samples to passband */
		sample  =  (float)cos(acc)*output.re;	
		sample -=  (float)sin(acc)*output.im;	
		
		//acc += (float)(2*PI*(M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);	

		if(psk_carrier_select == 1800)
		{
			acc += (float)(2*PI*(M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else if(psk_carrier_select == 1650)
		{
			acc += (float)(2*PI*(M2_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else if(psk_carrier_select == 1500)
		{
			acc += (float)(2*PI*(M3_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else
		{
			acc += (float)(2*PI*(M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}


		if( acc >= (float)(2*PI)) acc -= (float)(2*PI);

		current_tx_soundblock[current_sound_block_pointer++] = sample*0.5f;

		if( current_sound_block_pointer == tx_soundblock_size )
		{
			tx_sb_q.push(current_tx_soundblock);
			current_tx_soundblock = (float*)malloc(sizeof(float)*tx_soundblock_size);
			current_sound_block_pointer = 0;
		}
	}	
}
//
// Load the FIR filter with nothing.
//
void Cm110s::tx_silence( void )
{
	int i;
	FComplex sym;

	sym.re = sym.im = 0;

	for( i=0; i < M1_TX_FILTER_LENGTH/4; i++ ) tx_symbol( sym );
}
/*
 *
 * Transmit the preamble sequence.
 *
 */
void Cm110s::transmit_preamble_reset( void )
{
	tx_preamble_count        = 0;
	tx_data_scrambler_offset = 0;
}
void Cm110s::transmit_preamble( void )
{
	int i;

	for( i = 0; i < 480; i++ ) tx_symbol( tx_preamble[tx_preamble_count++] );
}
void Cm110s::transmit_unknown_data_symbol( int sym )
{
	sym = (sym+data_scrambler_bits[tx_data_scrambler_offset])%8;
	tx_symbol( con_symbol[sym] );	
	tx_data_scrambler_offset = ++tx_data_scrambler_offset%M1_DATA_SCRAMBLER_LENGTH; 
}
void Cm110s::tx_0_known_inter_block_data_symbols( void )
{
	int i;
	
	for( i=0; i < tx_known_data_length; i++ ) 
	{
		transmit_unknown_data_symbol( 0 );
	}
}
void Cm110s::tx_d1_known_inter_block_data_symbols( void )
{
	int i;
	
	for( i=0; i < tx_known_data_length; i++ ) 
	{
		transmit_unknown_data_symbol( tx_ib_seq_d1[i] );
	}
}
void Cm110s::tx_d2_known_inter_block_data_symbols( void )
{
	int i;
	
	for( i=0; i < tx_known_data_length; i++ ) 
	{
		transmit_unknown_data_symbol( tx_ib_seq_d2[i] );
	}
}
void Cm110s::tx_known_symbols( void )
{
	tx_block_count++;

	if( tx_block_count == (tx_block_count_mod-1))
	{
		tx_d1_known_inter_block_data_symbols();
	}
	else
	{
		if( tx_block_count == tx_block_count_mod )
		{
			tx_d2_known_inter_block_data_symbols();
			tx_block_count = 0;
		}
		else
		{
			tx_0_known_inter_block_data_symbols();
		}
	}
}
int Cm110s::transmit_interleaver_block( int length )
{
	int i,j,len,sym;
	Interleaver *lvr;

	lvr = &inblk[tx_current_interleaver_fetch];
 
	switch( tx_mode )
	{
		case M75NS:
		    len = length;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr )<<1;	
				sym += fetch_interleaver( lvr );
				sym  = mgd2[sym];
				tx_symbol_count++;
				if( tx_symbol_count == tx_block_count_mod )
				{
					for( j = 0; j < 32; j++ )
					{
						transmit_unknown_data_symbol(mes[sym][j]);
					}
					tx_symbol_count = 0;
				}
				else
				{
					for( j = 0; j < 32; j++ )
					{
						transmit_unknown_data_symbol(mns[sym][j]);
					}
				}								
			}
			break;
		case M75NL:
		    len = length;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr )<<1;	
				sym += fetch_interleaver( lvr );
				sym  = mgd2[sym];
				tx_symbol_count++;
				if( tx_symbol_count == tx_block_count_mod )
				{
					for( j = 0; j < 32; j++ )
					{
						transmit_unknown_data_symbol(mes[sym][j]);
					}
					tx_symbol_count = 0;
				}
				else
				{
					for( j = 0; j < 32; j++ )
					{
						transmit_unknown_data_symbol(mns[sym][j]);
					}
				}								
			}
			break;
		case M150S:
		case M150L:
		    len = length*8;
			for( i = 0 ; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr );	
				sym  = psk2[sym];
				transmit_unknown_data_symbol( sym );
				tx_symbol_count++;
				// Send know data 
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_symbol_count = 0; 
					tx_known_symbols();
				}
			}
			break;
		case M300S:
		case M300L:
		    len = length*4;
			for( i = 0 ; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr );	
				sym  = psk2[sym];
				transmit_unknown_data_symbol( sym );
				tx_symbol_count++;
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_symbol_count = 0; 
					tx_known_symbols();
				}
			}
			break;
		case M600S:
		case M600L:
		case M600V:
			len = length*2;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr );	
				sym  = psk2[sym];
				transmit_unknown_data_symbol( sym );
				tx_symbol_count++;
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_symbol_count = 0; 
					tx_known_symbols();
				}
			}
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			len = length;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr )<<1;	
				sym += fetch_interleaver( lvr );
				sym  = mgd2[sym];
				sym  = psk4[sym];
				transmit_unknown_data_symbol(sym);
				tx_symbol_count++;
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_known_symbols();
					tx_symbol_count = 0; 
				}
			}
			break;
		case M2400S:
		case M2400L:
		case M2400V:
		    len = (length*2)/3;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr )<<2;	
				sym += fetch_interleaver( lvr )<<1;	
				sym += fetch_interleaver( lvr );
				sym  = mgd3[sym];
				sym  = psk8[sym];
				transmit_unknown_data_symbol(sym);
				tx_symbol_count++;
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_symbol_count = 0; 
					tx_known_symbols();
				}
			}
			break;
		case M4800S:
		    len = length/3;
			for( i = 0; i < len; i++ )
			{
				sym  = fetch_interleaver( lvr )<<2;	
				sym += fetch_interleaver( lvr )<<1;	
				sym += fetch_interleaver( lvr );
				sym  = mgd3[sym];
				sym  = psk8[sym];
				transmit_unknown_data_symbol(sym);
				tx_symbol_count++;
				if( tx_symbol_count == tx_unknown_data_length )
				{
					tx_symbol_count = 0; 
					tx_known_symbols();
				}
			}
			break;
		default:
			break;
	}	
	if( lvr->nbits == 0 )
	{
		/* Interleaver empty. Switch blocks */
		tx_current_interleaver_fetch = tx_current_interleaver_fetch^1; 
		tx_symbol_count = 0; 
	}
	return( lvr->nbits );
}
void Cm110s::transmit_load_interleaver( int *bits, int length )
{
	int i;
	int bit1,bit2;
	Interleaver *lvr;	
	
	lvr = &inblk[tx_current_interleaver_load];
	//Effective Coding Rate 4800=None, 2400,1200,600,75 =1/2, 400=1/4, 150=1/8
	switch( tx_mode )
	{
		case M150S:
		case M150L:
			for( i = 0; i < length; i++ )
			{
				viterbi_encode( bits[i], &bit1, &bit2 );
				load_interleaver( bit1, lvr ); // repeated 4x
				load_interleaver( bit2, lvr );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
			}
			break;
		case M300S:
		case M300L:
			for( i = 0; i < length; i++ )
			{
				viterbi_encode( bits[i], &bit1, &bit2 );
				load_interleaver( bit1, lvr ); // repeated 2x
				load_interleaver( bit2, lvr );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
			}
			break;
		case M600S:
		case M600L:
		case M600V:
			for( i = 0; i < length; i++ )
			{
				viterbi_encode( bits[i], &bit1, &bit2 );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
			}
			break;
		case M75NS:
		case M75NL:
		case M1200S:
		case M1200L:
		case M1200V:
		case M2400S:
		case M2400L:
		case M2400V:
			for( i = 0; i < length; i++ )
			{
				viterbi_encode( bits[i], &bit1, &bit2 );
				load_interleaver( bit1, lvr );
				load_interleaver( bit2, lvr );
			}
			break;
		case M4800S:
			for( i = 0; i < length; i++ )
			{
				load_interleaver( bits[i], lvr );
			}
			break;
		default:
			break;
	}
}
void Cm110s::transmit_load_interleaver_block( int *bits, int length )
{
	Interleaver *lvr;

	lvr = &inblk[tx_current_interleaver_load];

	switch( tx_state )
	{
		case TX_IDLE_STATE:
			if( bits != NULL )
			{
				viterbi_encode_reset();
				tx_current_interleaver_load  = 0;
				tx_current_interleaver_fetch = 1;
				tx_block_count               = 0;
				tx_symbol_count              = 0;
				tx_silence();
				transmit_load_interleaver( bits, length );
				transmit_preamble_reset();
				transmit_preamble();
				ptt_active_count = 0;
				tx_state = TX_PREAMBLE_STATE;
			}
			break;
		case TX_PREAMBLE_STATE:
			if( bits != NULL )
			{
				transmit_preamble();
				transmit_load_interleaver( bits, length );
				if( lvr->size == lvr->nbits )
				{
					/* Interleaver full. Switch blocks */		
					tx_state = TX_DATA_STATE;
					tx_current_interleaver_load  = 1; 	
					tx_current_interleaver_fetch = 0; 	
					transmit_interleaver_block( length );
				}
			}
			break;
		case TX_DATA_STATE:
			if( bits != NULL )	transmit_load_interleaver( bits, length );
			if((inblk[0].nbits !=0 )||(inblk[1].nbits !=0 ))
			{
				// Still traffic to send.
				if( lvr->size == lvr->nbits )
				{
					/* Interleaver full. Switch blocks */
					tx_current_interleaver_load = (tx_current_interleaver_load^1); 	
				}
				transmit_interleaver_block( length );
			}
			else
			{
				tx_silence();
				tx_state = TX_CLOSING_STATE;
			}
			break;
		case TX_CLOSING_STATE:
			if( tx_sb_q.empty() )
			{
				if( ptt_active_count++ > 2 )
				{
					tx_state = TX_IDLE_STATE;
					func_status( TX_FALSE_STATUS, NULL );
				}
			}
			break;
		default:
			break;
	}
}
/*
 * Set the size of the outgoing soundblock.
 *
 */
void Cm110s::tx_set_soundblock_size( int soundblock_size )
{
	tx_soundblock_size          = soundblock_size;
	current_tx_soundblock       = (float*)malloc(sizeof(float)*tx_soundblock_size);
	current_sound_block_pointer = 0;
}
/*
 *
 * Get a soundblock to transmit.
 *
 */
float* Cm110s::tx_get_soundblock( void )
{

    float *ptr;

	ptr = 0;

	if( m_tx_enable != false )
	{
		if(!tx_sb_q.empty())
		{
			ptr = (float*)tx_sb_q.front();
			tx_sb_q.pop();
		}
	}
	return( ptr );	
}
/*
 *
 * Release the soundblock memory.
 *
 */
void Cm110s::tx_release_soundblock( float *block )
{
	free( block );
}
/*
void Cm110s::tx_psk_carrier( long carrier )
{
	psk_carrier_select = carrier;
}
*/
