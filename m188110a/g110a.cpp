/*
 * 
 * General module for 110A serial modem.
 *
 */
#include <math.h>
#include <stdio.h>
#include "platform_compat.h"
#include "Cm110s.h"






//
// Constructor.
//
Cm110s::Cm110s()
{
    init_preamble_module();
    create_data_scrambler_seq();
    create_prx_tables();
    create_mes_and_mns_seq();
    viterbi_init();
    viterbi_encode_reset();
    equalize_init();
    rx_callbk   = NULL;
    m_hold      = 1.0;
    m_tx_enable = false;
    m_rx_enable = false;


    rx_mode = M300L;

    sprintf_s( m_state_text,"IDLE");
}
//
// Destructor
//
Cm110s::~Cm110s()
{

}

void Cm110s::test()       //Test function
{

}





int * Cm110s::get_bits_to_send( int length )
{ 
	int *bp;
	
	if( out_ptr+length > in_ptr )
	{
		bp = (int *)0;
		in_ptr = 0;
	}
	else
	{
		bp      = &tx_bit_array[out_ptr];
		out_ptr = (out_ptr+length);
		if( out_ptr > M1_TX_BIT_ARRAY_LENGTH )
		{
			out_ptr = in_ptr = 0;
		}
	}
	return bp;
}

void Cm110s::output_rx_octet( unsigned char octet )
{
	// Reverse bit order - Viterbi decoder outputs LSB/MSB swapped
	unsigned char reversed = 0;
	reversed |= ((octet >> 0) & 1) << 7;
	reversed |= ((octet >> 1) & 1) << 6;
	reversed |= ((octet >> 2) & 1) << 5;
	reversed |= ((octet >> 3) & 1) << 4;
	reversed |= ((octet >> 4) & 1) << 3;
	reversed |= ((octet >> 5) & 1) << 2;
	reversed |= ((octet >> 6) & 1) << 1;
	reversed |= ((octet >> 7) & 1) << 0;
	
	rx_callbk( reversed );
}
//
// Set the transmit mode from a string.
//
void Cm110s::tx_set_mode( const char *txt )
{
	if( strcmp(txt,"75NS") == 0 ) tx_set_mode( M75NS );    
	if( strcmp(txt,"75NL") == 0 ) tx_set_mode( M75NL );   
	if( strcmp(txt,"150S") == 0 ) tx_set_mode( M150S );   
	if( strcmp(txt,"150L") == 0 ) tx_set_mode( M150L );   
	if( strcmp(txt,"300S") == 0 ) tx_set_mode( M300S );   
	if( strcmp(txt,"300L") == 0 ) tx_set_mode( M300L );   
	if( strcmp(txt,"600S") == 0 ) tx_set_mode( M600S );   
	if( strcmp(txt,"600L") == 0 ) tx_set_mode( M600L );
	if( strcmp(txt,"600V") == 0 ) tx_set_mode( M600V );
	if( strcmp(txt,"1200S") == 0 ) tx_set_mode( M1200S );  
	if( strcmp(txt,"1200L") == 0 ) tx_set_mode( M1200L );  
	if( strcmp(txt,"1200V") == 0 ) tx_set_mode( M1200V );
	if( strcmp(txt,"2400S") == 0 ) tx_set_mode( M2400S );  
	if( strcmp(txt,"2400L") == 0 ) tx_set_mode( M2400L );  
	if( strcmp(txt,"2400V") == 0 ) tx_set_mode( M2400V );  
	if( strcmp(txt,"4800S") == 0 ) tx_set_mode( M4800S );  

}
int Cm110s::get_number_of_bits_in_interleaver( Mode mode )
{
	int length;

	switch( mode )
	{
		case M75NS:
			length = 15*3;
			break;
		case M75NL:
			length = 15*24;
			break;
		case M150S:
			length = 30*3;
			break;
		case M150L:
			length = 30*24;
			break;
		case M300S:
			length = 60*3;
			break;
		case M300L:
			length = 60*24;
			break;
		case M600S:
			length = 120*3;
			break;
		case M600L:
			length = 120*24;
			break;
		case M600V:
			length = 120*3;
			break;
		case M1200S:
			length = 240*3;
			break;
		case M1200L:
			length = 240*24;
			break;
		case M1200V:
			length = 240*3;
			break;
		case M2400S:
			length = 480*3;
			break;
		case M2400L:
			length = 480*24;
			break;
		case M2400V:
			length = 480*3;
			break;
		case M4800S:
			length = 960*3;
			break;
		case MQUERYS:
		case MQUERYL:
		case MMAX:
		default:
			length = 0;
		    break;
	}
	return  length;
}
const char*  Cm110s::display_mode_string( Mode mode )
{
	static char msg[25];
	
	switch( mode )
	{
		case M75NS:
			sprintf_s(msg,"75 BPS SHORT");
			break;
		case M75NL:
			sprintf_s(msg,"75 BPS LONG");
			break;
		case M150S:
			sprintf_s(msg,"150 BPS SHORT");
			break;
		case M150L:
			sprintf_s(msg,"150 BPS LONG");
			break;
		case M300S:
			sprintf_s(msg,"300 BPS SHORT");
			break;
		case M300L:
			sprintf_s(msg,"300 BPS LONG");
			break;
		case M600S:
			sprintf_s(msg,"600 BPS SHORT");
			break;
		case M600L:
			sprintf_s(msg,"600 BPS LONG");
			break;
		case M600V:
			sprintf_s(msg,"600 BPS VOICE");
			break;
		case M1200S:
			sprintf_s(msg,"1200 BPS SHORT");
			break;
		case M1200L:
			sprintf_s(msg,"1200 BPS LONG");
			break;
		case M1200V:
			sprintf_s(msg,"1200 BPS VOICE");
			break;
		case M2400S:
			sprintf_s(msg,"2400 BPS SHORT");
			break;
		case M2400L:
			sprintf_s(msg,"2400 BPS LONG");
			break;
		case M2400V:
			sprintf_s(msg,"2400 BPS VOICE");
			break;
		case M4800S:
			sprintf_s(msg,"4800 BPS UNCODED");
			break;
		case MQUERYS:
			sprintf_s(msg,"QUERY SHORT");
			break;
		case MQUERYL:
			sprintf_s(msg,"QUERY LONG");
			break;
		case MMAX:
		    break;
		default:
		    break;
	}
	return msg;
}
//
// Publically visible.
//
const char*  Cm110s::rx_get_mode_string( void )
{
	return display_mode_string( rx_mode );
}
//
// Publically visible.
//
const char*  Cm110s::tx_get_mode_string( void )
{
	return display_mode_string( tx_mode );
}

Mode Cm110s::rx_get_mode( void )
{
    return( rx_mode );
}

/* Convert an octet array into a binary array, transmit MSB first */

void Cm110s::send_octet_array( unsigned char *octet, int length )
{

    int i,k;
		
	for( i = 0; i < length; i++ )
	{
		for( k = 7; k >= 0; k-- )
		{
			if(octet[i]&(1<<k))
				tx_bit_array[in_ptr] = 1;
			else
				tx_bit_array[in_ptr] = 0;				
			in_ptr = ++in_ptr%M1_TX_BIT_ARRAY_LENGTH;
		}	
	}
}
/* Convert an octet array into a binary array, transmit LSB first required for SYNC mode data */

void Cm110s::send_sync_octet_array( unsigned char *octet, int length )
{

    int i,k;
		
	for( i = 0; i < length; i++ )
	{
		for( k = 0; k <= 7; k++ )
		{
			if(octet[i]&(1<<k))
				tx_bit_array[in_ptr] = 1;
			else
				tx_bit_array[in_ptr] = 0;				
			in_ptr = ++in_ptr%M1_TX_BIT_ARRAY_LENGTH;
		}	
	}
}
void Cm110s::tx_bit( int bit )
{
	tx_bit_array[in_ptr] = bit;
	in_ptr = ++in_ptr%M1_TX_BIT_ARRAY_LENGTH;
}
void Cm110s::prepare_fresh_tx_samples( void )
{
    //qDebug()<<Q_FUNC_INFO<<"Entered"<<tx_get_mode_string();
    int length = number_of_bits_in_200ms(tx_get_mode());
	transmit_load_interleaver_block( get_bits_to_send( length ), length );
}
//
// Globally visible.
//
void Cm110s::tx_frame( unsigned char *data, int length )
{
	unsigned char eom[4]={0x4B,0x65,0xA5,0xB2};
	int i,i_block_length,i_to_send,total;
	char text[80];

	if( length == 0 ) return;
	if( m_tx_enable == false ) return;

	in_ptr = out_ptr = 0;

	func_status( TX_TRUE_STATUS, NULL );
	

	/* send message payload */
	send_octet_array( data, length );

	total = length;

	/* send EOM */
	send_octet_array( eom, 4 );

	//total += 4;

	/* Send flush bits */
	/* Flush decoder path length */

	for( i = 0; i < 176; i++ )
	{
		tx_bit( 0 );	
	}

	total += i;
	
	/* Calculate the number of bits in an interleaver block */

	i_block_length = get_number_of_bits_in_interleaver(tx_get_mode());

	// Fill the interleaver block

	i_to_send = i_block_length - (total%i_block_length);

	/* Send two interleaver blocks worth, this will always flush out the data */

	for( i = 0; i < i_block_length; i++ )
	{
		tx_bit( 0 );	
	}
	
	if( in_ptr > M1_TX_BIT_ARRAY_LENGTH )
	{
		sprintf_s(text,"Interleaver Flush ERROR! In pointer %d\r\n",in_ptr);
		func_status(TEXT_STATUS,text);
	}

	/* Now convert into sound */
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	do
	{
		prepare_fresh_tx_samples();
        Sleep(50);
	}
	while( tx_state != TX_IDLE_STATE );

}
void Cm110s::tx_data_port_sync_frame( unsigned char *data, int length )
{

	int i,i_block_length,i_to_send,total;
	char text[80];

	if( length == 0 ) return;
	if( m_tx_enable == false ) return;

	in_ptr = out_ptr = 0;

	func_status( TX_TRUE_STATUS, NULL );
	
	send_octet_array( data, length );


	total = length;

	// Send flush bits
	// Flush decoder path length

	for( i = 0; i < 176; i++ )
	{
		tx_bit( 0 );	
	}

	total += i;
	
	// Calculate the number of bits in an interleaver block 

	i_block_length = get_number_of_bits_in_interleaver(tx_get_mode());

	// Fill the interleaver block

	i_to_send = i_block_length - (total%i_block_length);

	// Send two interleaver blocks worth, this will always flush out the data

	for( i = 0; i < i_block_length; i++ )
	{
		tx_bit( 0 );	
	}
	
	if( in_ptr > M1_TX_BIT_ARRAY_LENGTH )
	{
		sprintf_s(text,"Interleaver Flush ERROR! In pointer %d\r\n",in_ptr);
		func_status(TEXT_STATUS,text);
	}

	// Now convert into sound
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	do
	{
		prepare_fresh_tx_samples();
		Sleep(50);
	}
	while( tx_state != TX_IDLE_STATE );

}
void Cm110s::tx_data_port_flush( int length )
{

	int i,i_block_length,i_to_send,total;
	char text[80];

	total = length;

	// Send flush bits
	// Flush decoder path length

	for( i = 0; i < 176; i++ )
	{
		tx_bit( 0 );	
	}

	total += i;
	
	// Calculate the number of bits in an interleaver block 

	i_block_length = get_number_of_bits_in_interleaver(tx_get_mode());

	// Fill the interleaver block

	i_to_send = i_block_length - (total%i_block_length);

	// Send two interleaver blocks worth, this will always flush out the data

	for( i = 0; i < i_block_length; i++ )
	{
		tx_bit( 0 );	
	}
	
	if( in_ptr > M1_TX_BIT_ARRAY_LENGTH )
	{
		sprintf_s(text,"Interleaver Flush ERROR! In pointer %d\r\n",in_ptr);
		func_status(TEXT_STATUS,text);
	}

	// Now convert into sound
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	do
	{
		prepare_fresh_tx_samples();
		Sleep(50);
	}
	while( tx_state != TX_IDLE_STATE );

}
void Cm110s::tx_sync_frame_eom( unsigned char *data, int length )
{

    // EOM is sent in this function
    unsigned char eom[4]={0x4B,0x65,0xA5,0xB2};

    int i,i_block_length,i_to_send,total;
    char text[80];
	if( length == 0 ) return;

    //qDebug()<<Q_FUNC_INFO<<"tx_enable = "<<m_tx_enable;

    if( m_tx_enable == false ) return;

	in_ptr = out_ptr = 0;

    func_status( TX_TRUE_STATUS, NULL );
	
	//send_octet_array( data, length );
    //qDebug()<<Q_FUNC_INFO<<"Calling send_sync_octet_array.";
    send_sync_octet_array( data, length );  //Reverse bit order for SYNC_EOM mode.

	total = length;

	// EOM is needed for SYNC EOM mode for ASCII and TRANSPARENT use
	/* send EOM */
	send_octet_array( eom, 4 );

	//total += 4;

	/* Send flush bits */
	/* Flush decoder path length */

	for( i = 0; i < 176; i++ )
	{
		tx_bit( 0 );	
	}

	total += i;
	
	/* Calculate the number of bits in an interleaver block */

	i_block_length = get_number_of_bits_in_interleaver(tx_get_mode());

	// Fill the interleaver block

	i_to_send = i_block_length - (total%i_block_length);

	/* Send two interleaver blocks worth, this will always flush out the data */

	for( i = 0; i < i_block_length; i++ )
	{
		tx_bit( 0 );	
	}
	
	if( in_ptr > M1_TX_BIT_ARRAY_LENGTH )
	{
		sprintf_s(text,"Interleaver Flush ERROR! In pointer %d\r\n",in_ptr);
		func_status(TEXT_STATUS,text);
	}

	/* Now convert into sound */
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	do
	{
		prepare_fresh_tx_samples();
		Sleep(50);
	}
	while( tx_state != TX_IDLE_STATE );

}
void Cm110s::tx_sync_frame( unsigned char *data, int length )
{
	// No EOM is sent in this function
	//unsigned char eom[4]={0x4B,0x65,0xA5,0xB2};
	int i,i_block_length,i_to_send,total;
	char text[80];

	if( length == 0 ) return;
	if( m_tx_enable == false ) return;

	in_ptr = out_ptr = 0;

	func_status( TX_TRUE_STATUS, NULL );
	
	//send_octet_array( data, length );
	send_sync_octet_array( data, length );

	total = length;

	// EOM not needed for SYNC mode
	/* send EOM */
	//send_octet_array( eom, 4 );  // << WHY REMARKED OUT

	/* Send flush bits */
	/* Flush decoder path length */

	for( i = 0; i < 176; i++ )
	{
		tx_bit( 0 );	
	}

	total += i;
	
	/* Calculate the number of bits in an interleaver block */

	i_block_length = get_number_of_bits_in_interleaver(tx_get_mode());

	// Fill the interleaver block

	i_to_send = i_block_length - (total%i_block_length);

	/* Send two interleaver blocks worth, this will always flush out the data */

	for( i = 0; i < i_block_length; i++ )
	{
		tx_bit( 0 );	
	}
	
	if( in_ptr > M1_TX_BIT_ARRAY_LENGTH )
	{
		sprintf_s(text,"Interleaver Flush ERROR! In pointer %d\r\n",in_ptr);
		func_status(TEXT_STATUS,text);
	}

	/* Now convert into sound */
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	prepare_fresh_tx_samples();
	do
	{
		prepare_fresh_tx_samples();
		Sleep(50);
	}
	while( tx_state != TX_IDLE_STATE );

}
void Cm110s::tx_frame( unsigned char *data, int length, Mode mode )
{

	tx_set_mode( mode );
    tx_sync_frame( data, length );
 
}
int Cm110s::number_of_bits_in_200ms( Mode mode )
{
	int length;
	
	switch( mode )
	{
		case M75NS:
		case M75NL:
			length = 15;
			break;
		case M150S:
		case M150L:
			length = 30;
			break;
		case M300S:
		case M300L:
			length = 60;
			break;
		case M600S:
		case M600L:
		case M600V:
			length = 120;
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			length = 240;
			break;
		case M2400S:
		case M2400L:
		case M2400V:
			length = 480;
			break;
		case M4800S:
			length = 960;
			break;
		case MQUERYS:
		case MQUERYL:
		case MMAX:
		default:
			length = 0;
		    break;
	}
	return length;
}
Mode Cm110s::tx_get_decremented_bitrate( void )
{	
	Mode mode;

	mode = tx_get_mode(); 

	switch( mode )
	{
		case M75NS:
		case M75NL:
			break;
		case M150S:
			mode = M75NS;
			break;
		case M150L:
			mode = M75NL;
			break;
		case M300S:
			mode = M150S;
			break;
		case M300L:
			mode = M150L;
			break;
		case M600S:
			mode = M300S;
			break;
		case M600L:
			mode = M300L;
			break;
		case M1200S:
			mode = M600S;
			break;
		case M1200L:
			mode = M600L;
			break;
		case M2400S:
			mode = M1200S;
			break;
		case M2400L:
			mode = M1200L;
			break;
		case M600V:
			mode = M1200S;  // No manual selection
			break;
		case M1200V:
			mode = M1200S;  // No manual selection
			break;
		case M2400V:
			mode = M1200S;  // No manual selection
			break;
		case M4800S:
			mode = M2400L;
			break;
		default:
		    break;
	}
	return( mode );
}
Mode Cm110s::tx_get_incremented_bitrate( void )
{	
	Mode mode;

	mode = tx_get_mode();

	switch( mode )
	{
		case M75NS:
			mode = M150S;
			break;
		case M75NL:
			mode = M150L;
			break;
		case M150S:
			mode = M300S;
			break;
		case M150L:
			mode = M300L;
			break;
		case M300S:
			mode = M600S;
			break;
		case M300L:
			mode = M600L;
			break;
		case M600S:
			mode = M1200S;
			break;
		case M600L:
			mode = M1200L;
			break;
		case M600V:
			mode = M1200S;
			break;
		case M1200S:
			mode = M2400S;
			break;
		case M1200L:
			mode = M2400L;
			break;
		case M1200V:
			mode = M2400S;
			break;
		case M2400S:
			mode = M4800S;
			break;
		case M2400L:
			mode = M4800S;
			break;
		case M2400V:
			mode = M4800S;
			break;
		case M4800S:
			mode = M4800S;
			break;
		default:
		    break;
	}
	return( mode );
}
Mode Cm110s::tx_get_toggled_interleave( void )
{	
	Mode mode;

	mode = tx_get_mode(); 

	switch( mode )
	{
		case M75NS:
			mode = M75NL;
			break;
		case M75NL:
			mode = M75NS;
			break;
		case M150S:
			mode = M150L;
			break;
		case M150L:
			mode = M150S;
			break;
		case M300S:
			mode = M300L;
			break;
		case M300L:
			mode = M300S;
			break;
		case M600S:
			mode = M600L;
			break;
		case M600L:
			mode = M600S;
			break;
		case M600V:
			mode = M600V;
			break;
		case M1200S:
			mode = M1200L;
			break;
		case M1200L:
			mode = M1200S;
			break;
		case M1200V:
			mode = M1200V;
			break;
		case M2400S:
			mode = M2400L;
			break;
		case M2400L:
			mode = M2400S;
			break;
		case M2400V:
			mode = M2400V;
			break;
		case M4800S:
			mode = M4800S;
			break;
		default:
		    break;
	}
	return( mode );
}
void Cm110s::register_receive_octet_callback_function(void(*callback)(unsigned char ))
{
    //qDebug()<<Q_FUNC_INFO<<callback<<QThread::currentThreadId();
    (this->rx_callbk) = callback;
}
void Cm110s::register_status(void(*status)(ModemStatus,void*))
{
    //qDebug()<<Q_FUNC_INFO<<status<<QThread::currentThreadId();
    func_status = status;
}
// Constellation routines
//
// Called to read received data
//
int Cm110s::ReadConstellation( FComplex *in, int length )
{

	int l = m_c_q.size();

	if(m_c_q.empty())
	{
		//l = 0;
		return l;
	}

	if(l !=0)
	{
		if( l > length ) l = length;

		for( int i = 0; i < l; i++ )
		{

			if(m_c_q.empty())
			{
				//do nothing
			}
			else
			{

			in[i] = m_c_q.front();
			
			m_c_q.pop();


			if(m_c_q.empty())
			{
				//l = 0;
				return l;
			}

			}
		}
		
		return l;
	}
	else
	{
		//l = 0;
		return l;	
	}

}
// Post Equalization
int Cm110s::ReadConstellationPeq( FComplex *in, int length )
{
	int l = m_c_q_peq.size();

	if(m_c_q_peq.empty())
	{
		//l = 0;
		return l;
	}

	if(l !=0)
	{
		if( l > length ) l = length;

		for( int i = 0; i < l; i++ )
		{

			if(m_c_q_peq.empty())
			{
				//do nothing
			}
			else
			{

			in[i] = m_c_q_peq.front();

			m_c_q_peq.pop();

			if(m_c_q_peq.empty())
			{
				return l;
			}

			}
		}
		
		return l;
	}
	else
	{
		//l = 0;
		return l;	
	}
}
