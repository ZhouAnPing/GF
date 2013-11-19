! GF - Start of CIM System custom modifications
!----------------------------------------------------------------------------

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! THIS FILE WAS CREATED TO PROVIDE CUSTOM STATUS BOARD DATA
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	 program MCSSYNC

! Description: This program will retrieve data from the MCS server and
!              populate the Promis global sections.

	 implicit none

! Author:	08-Feb-1996	Sebastien Spicer

! Modifications:
!

! Include files:

	 include 'FIL:TYPDEF.FOR'			! sizing parameters
	 include 'TTC:TTCMSG'
	 include 'WCT:WCTDEF'
	 include 'WCT:WIPLOTCOM'

! Routines used
C	!subroutine Cfc_Imginitlz
C	!subroutine lib$establish
C	!subroutine Tut_SetInput_Off
	 character*(TYP__USERIDSZ) Ssp_Option_ServerAccount
	 integer*4 Uaf_User_DetLogin
C	!subroutine SspStat_McsSync_LockTables
C	!subroutine Wct_LockTable_ReleaseLocks
	 integer*4 Gut_TrimBlks
	 external Ttc_TpHandler

! Local variables

	 character*(TYP__LOTIDSZ) lotid(WCT__MAXWIPLOT)
	 character*(TYP__MCSLOCSZ) mcsLocation(WCT__MAXWIPLOT)

	 character*(TYP__USERIDSZ) account
	 integer*4 i, mark, mcsCount,gbl_lotid_no
	 integer*4 status
	 integer*4 psize
	 character*12 processName
	 character*1 junk
	 integer*2 iostat_i2

	 integer*4  Lot_send_i4
	 integer*4  start_lot_i4
	 integer*4  lots_processed_i4
C
C	DELAY TIME IN SECONDS

	 PARAMETER MAX_LOST_FOR_Q = 750

	 integer pos

        integer*2 timeout_try_i2

	 integer*2	delay_i2

	 real delay_r4

	 integer*2 max_timeout_try_i2

	 character*132 err_msg_c132

	 character*(TYP__MCSLOCSZ) timeout_str

	 logical*1 found_l1

	 integer*4	decq_timeout_i4
	 integer*4	verbose_i4
	 integer*4	q_len_i4

	 character*80 tmp_str

	 character*80 dec_msg_q_name

	 integer*4	index

	 integer*4	substring_index

	 character*255 command_string
	 integer*4 command_len
! Code

	 do pos=1,TYP__MCSLOCSZ
	   timeout_str(pos:pos)='?'
	 end do

C******************************************************************
C       Default values for Dec Msg Q Name and Wait time in seconds
C******************************************************************

	 dec_msg_q_name='MCS'

	 q_len_i4 = 3

	 delay_r4 = 60.0             ! Delay for looping
	
	 decq_timeout_i4 = 60        ! DEC MSG Q TIMEOUT ( second )

	 max_timeout_try_i2 = 3      ! NUMBER OF TRIES BEFORE ??????

	 verbose_i4 = 0              ! for Trace

C*******************************************************
C  Read COmmand line arguments and parse them
C  not given take defaults
C*******************************************************

	 call lib$get_foreign( command_string,,command_len,)


C       If command line argument is given then  check if time is
C       mentioned in it 

	 call str$upcase(command_string,command_string)

	 if ( command_len .gt. 0 ) then

C
C       Check if timeout is given
C
	  call str$find_first_substring ( command_string,index,
	1		substring_index,
	1		'-DELAY' )

	  if ( index .gt. 0 ) then
	    tmp_str=' '
	    pos=index+7          ! "-delay" 
	    substring_index=0
	    do while ( ( command_string(pos:pos) .ne. ' ')  .and.
	1	     ( pos .le. command_len ) )
	     	substring_index = substring_index + 1
		tmp_str(substring_index:substring_index) = 
	1		command_string(pos:pos)	       
		pos=pos+1
	    end do
	    if ( substring_index .gt. 0 ) then
		delay_i2=0
		read ( tmp_str(1:substring_index),fmt='(i)',
	1	iostat=iostat_i2)delay_i2
		if ( iostat_i2 .eq. 0 .and. delay_i2 .gt. 0 ) then
		  delay_r4 = delay_i2
		end if
	    endif 

	  end if

C	type *,'DELAY:',delay_r4
C
C       Check if q name is given 
C

	  call str$find_first_substring ( command_string,index,
	1		substring_index,
	1		'-QNAME' )

	  if ( index .gt.  0 ) then
	    tmp_str=' '
	    pos=index+7          ! "-qname" 
	    substring_index=0
	    do while ( ( command_string(pos:pos) .ne. ' ') .and.
	1	     ( pos .le. command_len ) )
	     	substring_index = substring_index + 1
		tmp_str (substring_index:substring_index) = 
	1		command_string(pos:pos)	       
		pos=pos+1
	    end do
	    if ( substring_index .gt. 0 ) then
		  dec_msg_q_name = tmp_str(1:substring_index)
		  q_len_i4 = substring_index
	    endif 

	  end if

C	type *,'QNAME:',dec_msg_q_name	    
C
C       Check if retryn name is given 
C

	  call str$find_first_substring ( command_string,index,
	1		substring_index,
	1		'-RETRY' )

	  if ( index .gt. 0 ) then
	    tmp_str=' '
	    pos=index+7          ! "-retry" 
	    substring_index=0
	    do while ( ( command_string(pos:pos) .ne. ' ') .and. 
	1	     ( pos .le. command_len ) )
	     	substring_index = substring_index + 1
		tmp_str (substring_index:substring_index) = 
	1		command_string(pos:pos)	    
		pos=pos+1   
	    end do
	    if ( substring_index .gt. 0 ) then
		delay_i2=0
		read ( tmp_str(1:substring_index),fmt='(i)',
	1	iostat=iostat_i2)delay_i2
		if ( iostat_i2 .eq. 0 .and. delay_i2 .gt. 0 ) then
		  max_timeout_try_i2 = delay_i2
		end if
	    endif 

	  end if

C	type *,'max timeout try',max_timeout_try_i2	    
C
C       Check if qdelay name is given 
C

	  call str$find_first_substring ( command_string,index,
	1		substring_index,
	1		'-QDELAY' )

	  if ( index .gt. 0 ) then
	    tmp_str=' '
	    pos=index+8          ! "-qdelay" 
	    substring_index=0
	    do while ( ( command_string(pos:pos) .ne. ' ') .and.
	1	     ( pos .le. command_len ) )
	     	substring_index = substring_index + 1
		tmp_str (substring_index:substring_index) = 
	1		command_string(pos:pos)	       
		pos=pos+1
	    end do
	    if ( substring_index .gt. 0 ) then
		delay_i2=0
		read ( tmp_str(1:substring_index ),fmt='(i)',
	1	iostat=iostat_i2)delay_i2
		if ( iostat_i2 .eq. 0 .and. delay_i2 .gt. 0 ) then
	          decq_timeout_i4 = delay_i2
		end if
	    endif 

	  end if
C	type *,'DEC Q timeout try',decq_timeout_i4
C
C       Check if verbose name is given 
C

	  call str$find_first_substring ( command_string,index,
	1		substring_index,
	1		'-VERBOSE' )

	  if ( index .gt. 0 ) then
	    pos=index+9          ! "-verbose" 
	    substring_index=0
	    do while ( ( command_string(pos:pos) .ne. ' ') .and. 
	1	     (pos .le. command_len ) )
	     	substring_index = substring_index + 1
		tmp_str (substring_index:substring_index) = 
	1		command_string(pos:pos)	       
		pos=pos+1
	    end do
	    if ( substring_index .gt. 0 ) then
		delay_i2=0
		read ( tmp_str(1:substring_index),fmt='(i)',
	1	iostat=iostat_i2)delay_i2
		if ( iostat_i2 .eq. 0 .and. delay_i2 .gt. 0 ) then
	          verbose_i4 = delay_i2
		end if
	    endif 

	  end if
	    

C	type *,'VEROSE',verbose_i4
	 end if         ! END OF IF COMMAND LINE WAS GIVEN

C*******************************************************
C	! PROMIS initialization
C*********************************************************

	 call Cfc_Imginitlz (0)			! no local I/O for now (0 arg)
	 call lib$establish (Ttc_TpHandler)	! establish handler
	 call Tut_SetInput_Off ()			! barff up if input requested

C	! Get name of shared account and attempt to login

	 account = Ssp_Option_ServerAccount ()
	 status = Uaf_User_DetLogin (account)
	 if (.not. status) goto 1000

C********************************************************
C	INITIALIZE THE DEC MESSAGE Q
C********************************************************

	 call init_decmsg_q(%ref(dec_msg_q_name), %ref(q_len_i4),
	1	%ref(verbose_i4),%ref(decq_timeout_i4),%ref(status),
	1	%ref(err_msg_c132) )

	 if ( status .ne. 0 ) then
	  write ( 6,fmt='(" ERROR INITIALIZING THE DEC. MSG. Q ")')
	  write ( 6,fmt='(" REASON::",A,/,x,a)')err_msg_c132(1:60),
	1	err_msg_c132(61:132)
	  goto 1000
	 else
	  write ( 6,fmt='(" INITIALIZING THE DEC. MSG. Q SUCCESSFULLY")')
	 end if


	  timeout_try_i2 = 0

c**********************************************************
C	Top of client loop - repeat forever (with pause)
C***********************************************************        

	 do while (.true.)

	  ! Lock the tables and make a copy of the WIP list
       write( 6,*) '*****New Loop after Time Interval***'

	  call SspStat_McsSync_LockTables ()

	  do gbl_lotid_no = 1, wct_wipLotCom_maxIndexUsed
	     lotId( gbl_lotid_no ) = wct_wipLotCom_lotId(gbl_lotid_no)
	  end do

	  mcsCount = wct_wipLotCom_maxIndexUsed

	  call Wct_LockTable_ReleaseLocks

	  ! Get the MCS information for the WIP list

       write ( 6,*) 'promis Lot count=',mcsCount

	  start_lot_i4 = 1

	  lots_processed_i4=0

	  do while ( lots_processed_i4 .lt. mcscount )

	    lot_send_i4 = min( MAX_LOST_FOR_Q, (mcsCount - start_lot_i4 + 1) )
C		type *,'MCS ',mcscount ,'lot_send ',lot_send_i4

C		type *,'processed',lots_processed_i4, 'start',start_lot_i4
  	    call get_mcs_info( %ref(lot_send_i4),
	1		      %ref(LotId(start_lot_i4)),
	1		      %ref(mcsLocation(start_lot_i4)),
	1		      %ref(status),
	1		      %ref(err_msg_c132) )

	    if ( status .ne. 0 ) then ! FAILURE

	      lots_processed_i4 = mcsCount

	      write ( 6,fmt='(" ERROR GETTING LOT INFO ")')

	      write ( 6,fmt='(" REASON::",A,/,x,a)')err_msg_c132(1:60),
	1	err_msg_c132(61:132)

	      timeout_try_i2 = timeout_try_i2 + 1

C***********************************************************
C          If no of timeouts is greater than the maximum set
C          then put timeout marks
C***********************************************************

              if ( timeout_try_i2 .gt. max_timeout_try_i2) then

	        timeout_try_i2 = 0

	        write(6,fmt='(" MAX # OF TIMOUTS OCCURED. ")')

C	        call SspStat_McsSync_LockTables

C	        do gbl_lotid_no = 1, wct_wipLotCom_maxIndexUsed

C	          wct_wipLotCom_mcsLocation( gbl_lotid_no ) = timeout_str

C	        end do

C	        call Wct_LockTable_ReleaseLocks
     
	      end if

	    else                   ! SUCCESS

              lots_processed_i4 = lots_processed_i4 + lot_send_i4

              start_lot_i4 = start_lot_i4 + lot_send_i4

	      timeout_try_i2=0
          
	  ! Lock the tables and store the location

	      call SspStat_McsSync_LockTables


C***********************************************************
C   If global lot id is matching the lot id for which location
C   was retrieved then store the location 
C
C   If Global Location is equal to last marked location ( identified by mark)
C   then store else search thru all the lots to check for matching
C   lot id 
C************************************************************

  	      mark = 1

	      do gbl_lotid_no = 1, wct_wipLotCom_maxIndexUsed

	        if ( wct_wipLotCom_lotId( gbl_lotid_no ) .EQ. 
	1	   lotId(mark) ) then
               if(len(trim(mcsLocation(mark))) .GT. 0) then
	           wct_wipLotCom_mcsLocation( gbl_lotid_no ) = mcsLocation(mark)
	          end if
C               write( 6,*) '*******Sequnce****'
C               write( 6,*) 'Lot No. =',gbl_lotid_no
C               write( 6,*) 'promis lot =',lotId(mark)
C               write( 6,*) 'location =',mcsLocation(mark)
C               write( 6,*) '******************'
	          mark = mark + 1

	        else

	          mark = 1

                  found_l1 = .FALSE.

	          do while ( (mark .LE. mcsCount) .AND. (.not. found_l1 ))

		    if ( wct_wipLotCom_lotId( gbl_lotid_no) 
	1	       .eq. lotId(mark) ) then

                  if(len(trim(mcsLocation(mark))) .GT. 0) then
	               wct_wipLotCom_mcsLocation(gbl_lotid_no) =mcsLocation(mark)
                  end if
C                   write( 6,*) '*****NonSequnce***'
C                   write( 6,*) 'Lot No. =',gbl_lotid_no
C                   write( 6,*) 'promis lot =',lotId(mark)
C                   write( 6,*) 'location =',mcsLocation(mark)
C                   write( 6,*) '******************'

		      found_l1 = .true.              

		    end if

	            mark = mark + 1

	          end do

	          if ( .not. found_l1 ) mark=gbl_lotid_no+1

	        end if

	      end do ! END OF ALL LOTS SEARCH 

	      call Wct_LockTable_ReleaseLocks

	    end if   ! end of get mcs info success 

	  end do  ! END OF ALL LOTS PROCESSED 

	  call lib$wait(delay_r4)

	 end do   !    END OF LOOP FOR EVER


C	! Come here when exiting server
1000	 continue
C	! Tell PROMIS console that we are shutting down with an error
	 if (status .ne. TTC__SERVEREXIT) then
C	  ! Get name of this process
	  call Cfc_ProcInfo (processName, junk, junk, junk)
	  psize = Gut_TrimBlks (processName)
	  call Cfc_SendOper ('Server:' // processName(:psize) //
     &                       ' shutting down with error')
        endif

	 call Uaf_User_DetLogOut ()		! log off PROMIS
	 call Lib$Do_Command ('EXIT')	   	! EXIT from command file
	 end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	 subroutine SspStat_McsSync_LockTables

! Description: This function will lock the WIP tables

	 implicit none

! Author:	08-Feb-1996	Sebastien Spicer

! Modifications:
!

! Include files:

! Routines used
	 logical*4 Wct_LockTable_ReadLock
C	!subroutine Wct_LockTable_ReleaseLocks

! Local variables
	 logical*4 gotLock

! Code

C	! Get lock on workcenter table.
	 gotLock = .false.
	 do while (.not. gotLock)
	  gotLock = Wct_LockTable_ReadLock()
	  if (gotLock) then
C	    ! no deadlock, lock obtained successfully.
	  else
C	    ! deadlock detected. Release all locks and try again.
	    call Wct_LockTable_ReleaseLocks
	  end if
	 end do

	 return
	 end

!----------------------------------------------------------------------------
! GF - Start of CIM System custom modifications
