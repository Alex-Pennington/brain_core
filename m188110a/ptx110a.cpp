/*
 *
 * MIL-STD 110A serial tone modem.
 *
 * TX preamble routines.
 *
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "Cm110s.h"

extern int      psymbol[8][32];
extern FComplex con_symbol[8];
extern int      pscramble[32];
extern int      p_c_seq[9];

int Cm110s::fcomplex_copy( FComplex *dest, FComplex *src, int length )
{
	memcpy(dest,src,sizeof(FComplex)*length);
	return length;
}
//
// Used on transmit.
//
void Cm110s::set_known_inter_block_seq( int d1, int d2 )
{
	int i;
	
	for( i = 0; i < 8; i++ ) tx_ib_seq_d1[i]    = psymbol[d1][i];
	for( i = 0; i < 8; i++ ) tx_ib_seq_d1[i+8]  = psymbol[d1][i];
	for( i = 0; i < 4; i++ ) tx_ib_seq_d1[i+16] = 0;

	for( i = 0; i < 8; i++ ) tx_ib_seq_d2[i]    = psymbol[d2][i];
	for( i = 0; i < 8; i++ ) tx_ib_seq_d2[i+8]  = psymbol[d2][i];
	for( i = 0; i < 4; i++ ) tx_ib_seq_d2[i+16] = 0;

}
//
// Used on receive.
//
void Cm110s::build_known_ib_seq( int d1, int d2, int length )
{
	int i;
	
	for( i = 0; i < 8; i++ ) rx_ib_seq_d1[i]    = con_symbol[psymbol[d1][i]];
	for( i = 0; i < 8; i++ ) rx_ib_seq_d1[i+8]  = con_symbol[psymbol[d1][i]];
	for( i = 0; i < 4; i++ ) rx_ib_seq_d1[i+16] = con_symbol[0];	

	for( i = 0; i < 8; i++ ) rx_ib_seq_d2[i]    = con_symbol[psymbol[d2][i]];
	for( i = 0; i < 8; i++ ) rx_ib_seq_d2[i+8]  = con_symbol[psymbol[d2][i]];
	for( i = 0; i < 4; i++ ) rx_ib_seq_d2[i+16] = con_symbol[0];	

}
int Cm110s::add_count_seq( int count, int *buff )
{
	buff[2] = (count&0x03)+4;
	count >>= 2;
	buff[1] = (count&0x03)+4;
	count >>= 2;
	buff[0] = (count&0x03)+4;
	
	return 3;
}
int  Cm110s::add_common_preamble_seq( int *buff )
{
	for( int i = 0; i < 9; i++ ) buff[i] = p_c_seq[i];
	
	return 9;
}
int Cm110s::create_common_preamble_sequence( FComplex *buff )
{
	int seqa[9];
	int seqb[288];
	int i,j,k,index;
	int length;
	
	/* Add the common sequence */
	length = add_common_preamble_seq( seqa );
	
	/* Encode into preamble symbols */

	for( i = 0,index = 0; i < length; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			for( k = 0; k < 8; k++ )
			{
				seqb[index++] = psymbol[seqa[i]][k];
			}
		}
	}

	/* Apply scrambling seq */

	for( i=0; i<index; i++ )
	{
		seqb[i] = (seqb[i]+pscramble[i%32])%8;
	}	

	/* Convert into channel symbols */
	
	for( i = 0; i < index; i++ )
	{
		buff[i] = con_symbol[seqb[i]];
	}
	return index;	
}
int Cm110s::create_d_preamble_sequence( int d, FComplex *buff )
{
	int seq[32];
	int i,j,k,index;
	
	index = 0;
		
	/* Encode into preamble symbols */

	for( j = 0; j < 4; j++ )
	{
		for( k = 0; k < 8; k++ )
		{
			seq[index++] = psymbol[d][k];
		}
	}

	/* Apply scrambling seq */

	for( i=0; i<index; i++ )
	{
		seq[i] = (seq[i]+pscramble[i%32])%8;
	}	

	/* Convert into channel symbols */
	
	for( i = 0; i < index; i++ )
	{
		buff[i] = con_symbol[seq[i]];
	}
	return index;	
}
int Cm110s::create_count_preamble_sequence( int count, FComplex *buff )
{
	int seqa[3];
	int seqb[96];
	int i,j,k,index;
	
	add_count_seq(count,seqa);
		
	/* Encode into preamble symbols */

	for( i = 0,index = 0; i < 3; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			for( k = 0; k < 8; k++ )
			{
				seqb[index++] = psymbol[seqa[i]][k];
			}
		}
	}

	/* Apply scrambling seq */

	for( i=0; i<index; i++ )
	{
		seqb[i] = (seqb[i]+pscramble[i%32])%8;
	}	

	/* Convert into channel symbols */
	
	for( i = 0; i < index; i++ )
	{
		buff[i] = con_symbol[seqb[i]];
	}
	return index;	
}
int Cm110s::create_zero_preamble_sequence( FComplex *buff )
{
	int seqb[32];
	int i,j,k,index;

	index = 0;
		
	for( j = 0; j < 4; j++ )
	{
		for( k = 0; k < 8; k++ )
		{
			seqb[index++] = psymbol[0][k];
		}
	}

	/* Apply scrambling seq */

	for( i=0; i<index; i++ )
	{
		seqb[i] = (seqb[i]+pscramble[i%32])%8;
	}	

	/* Convert into channel symbols */
	
	for( i = 0; i < index; i++ )
	{
		buff[i] = con_symbol[seqb[i]];
	}
	return index;	
}
int Cm110s::create_short_preamble( Mode mode, FComplex *buff )
{
	int offset;
	int i,c;
	
	offset = 0;

	for( i=0,c=2; i<3; i++,c-- )
	{
		offset += fcomplex_copy( &buff[offset], p_common,     M1_P_COMMON_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_mode[mode], M1_P_MODE_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_count[c],   M1_P_COUNT_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_zero,       M1_P_ZERO_LENGTH );
	}
	return offset;
}
int Cm110s::create_long_preamble( Mode mode, FComplex *buff )
{
	int offset;
	int i,c;
	
	offset = 0;

	for( i=0,c=23; i<24; i++,c-- )
	{
		offset += fcomplex_copy( &buff[offset], p_common,     M1_P_COMMON_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_mode[mode], M1_P_MODE_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_count[c],   M1_P_COUNT_LENGTH );
		offset += fcomplex_copy( &buff[offset], p_zero,       M1_P_ZERO_LENGTH );
	}
	return offset;
}
int Cm110s::create_preamble_and_known_data( Mode mode, FComplex *buff )
{
	int length=0;
	
	switch(mode)
	{
		case M75NS:
			length = create_short_preamble( mode, buff );
			//set_known_inter_block_seq( 7, 5 );// add as test
			//tx_known_data_length   = 0; // add as test
			//tx_unknown_data_length = 0; // add as test
			break;
		case M75NL:
			length = create_long_preamble( mode, buff );
			//set_known_inter_block_seq( 5, 5 );// add as test
			//tx_known_data_length   = 0; // add as test
			//tx_unknown_data_length = 0; // add as test
			break;
		case M150S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 7, 4 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M150L:
			length = create_long_preamble( mode, buff );
			set_known_inter_block_seq( 5, 4 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M300S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 6, 7 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M300L:
			length = create_long_preamble( mode, buff );
			set_known_inter_block_seq( 4, 7 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M600S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 6, 6 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M600L:
			length = create_long_preamble( mode, buff );
			set_known_inter_block_seq( 4, 6 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M600V:
			length = create_short_preamble(mode, buff);
			set_known_inter_block_seq( 5, 7);
			tx_known_data_length = 20;
			tx_unknown_data_length = 20;
			break;
		case M1200S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 6, 5 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M1200L:
			length = create_long_preamble( mode, buff );
			set_known_inter_block_seq( 4, 5 );
			tx_known_data_length   = 20;
			tx_unknown_data_length = 20;
			break;
		case M1200V:
			length = create_short_preamble(mode, buff);
			set_known_inter_block_seq( 5, 6 );
			tx_known_data_length = 20;
			tx_unknown_data_length = 20;
			break;
		case M2400S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 6, 4 );
			tx_known_data_length   = 16;
			tx_unknown_data_length = 32;
			break;
		case M2400L:
			length = create_long_preamble( mode, buff );
			tx_known_data_length   = 16;
			tx_unknown_data_length = 32;
			set_known_inter_block_seq( 4, 4 );
			break;
		case M2400V:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 7, 7 );
			tx_known_data_length   = 16;
			tx_unknown_data_length = 32;
			break;
		case M4800S:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 7, 6 );
			tx_known_data_length   = 16;
			tx_unknown_data_length = 32;
			break;
		case MQUERYS:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 5, 6 );
			break;
		case MQUERYL:
			length = create_short_preamble( mode, buff );
			set_known_inter_block_seq( 5, 7 );
			break;
		default:
			break;
	}
	return(length);
}
void Cm110s::init_preamble_module( void )
{
	int i;
	/* Create known sequences for use by the receiver */
	create_common_preamble_sequence( p_common );

	/* M75NS */
	create_d_preamble_sequence( 7, &p_mode[M75NS][0]  );
	create_d_preamble_sequence( 5, &p_mode[M75NS][32] );
	/* M75NL */
	create_d_preamble_sequence( 5, &p_mode[M75NL][0]  );
	create_d_preamble_sequence( 5, &p_mode[M75NL][32] );
	/* M150S */
	create_d_preamble_sequence( 7, &p_mode[M150S][0]  );
	create_d_preamble_sequence( 4, &p_mode[M150S][32] );
	/* M150L */
	create_d_preamble_sequence( 5, &p_mode[M150L][0]  );
	create_d_preamble_sequence( 4, &p_mode[M150L][32] );
	/* M300S */
	create_d_preamble_sequence( 6, &p_mode[M300S][0]  );
	create_d_preamble_sequence( 7, &p_mode[M300S][32] );
	/* M300L */
	create_d_preamble_sequence( 4, &p_mode[M300L][0]  );
	create_d_preamble_sequence( 7, &p_mode[M300L][32] );
	/* M600S */
	create_d_preamble_sequence( 6, &p_mode[M600S][0]  );
	create_d_preamble_sequence( 6, &p_mode[M600S][32] );
	/* M600v */
	create_d_preamble_sequence( 5, &p_mode[M600V][0]);
	create_d_preamble_sequence( 7, &p_mode[M600V][32]);
	/* M600L */
	create_d_preamble_sequence( 4, &p_mode[M600L][0]  );
	create_d_preamble_sequence( 6, &p_mode[M600L][32] );
	/* M1200S */
	create_d_preamble_sequence( 6, &p_mode[M1200S][0]  );
	create_d_preamble_sequence( 5, &p_mode[M1200S][32] );
	/* M1200L */
	create_d_preamble_sequence( 4, &p_mode[M1200L][0]  );
	create_d_preamble_sequence( 5, &p_mode[M1200L][32] );
	/* M1200V */
	create_d_preamble_sequence( 5, &p_mode[M1200V][0]);
	create_d_preamble_sequence( 6, &p_mode[M1200V][32]);
	/* M2400S */
	create_d_preamble_sequence( 6, &p_mode[M2400S][0]  );
	create_d_preamble_sequence( 4, &p_mode[M2400S][32] );
	/* M2400L */
	create_d_preamble_sequence( 4, &p_mode[M2400L][0]  );
	create_d_preamble_sequence( 4, &p_mode[M2400L][32] );
	/* M2400V */
	create_d_preamble_sequence( 7, &p_mode[M2400V][0]  );
	create_d_preamble_sequence( 7, &p_mode[M2400V][32] );
	/* M4800S */
	create_d_preamble_sequence( 7, &p_mode[M4800S][0]  );
	create_d_preamble_sequence( 6, &p_mode[M4800S][32] );
	/* MQUERYS */
	create_d_preamble_sequence( 5, &p_mode[MQUERYS][0]  );
	create_d_preamble_sequence( 6, &p_mode[MQUERYS][32] );
	/* MQUERYL */
	create_d_preamble_sequence( 5, &p_mode[MQUERYL][0]  );
	create_d_preamble_sequence( 7, &p_mode[MQUERYL][32] );
	/* Sequences for count */
	for( i = 0; i < 24; i++ )
	{
		create_count_preamble_sequence( i, p_count[i] );	
	}
	/* Create zero sequence */
	create_zero_preamble_sequence( p_zero );
}