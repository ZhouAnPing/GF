! GF - Start of CIM System custom modifications
!----------------------------------------------------------------------------

!----------------------------------------------------------------------------
       subroutine updateLotLocInPROMIS(lotId, mcsLocation,lotId_len,mcsLoc_len)

! Description: This function will update promis global section for certain lot

       implicit none

! Author: 15-July-2013    Zhou An-Ping

! Modifications:
!

! Include files:

      include 'FIL:TYPDEF.FOR'               ! sizing parameters
      include 'TTC:TTCMSG'
      include 'WCT:WCTDEF'
      include 'WCT:WIPLOTCOM'

C      character*(TYP__USERIDSZ) Ssp_Option_ServerAccount
C      integer*4 Uaf_User_DetLogin
C      external Ttc_TpHandler
C      character*(TYP__USERIDSZ) account

C      integer*4 status


       BYTE          lotId(TYP__LOTIDSZ)
       BYTE          mcsLocation(TYP__MCSLOCSZ)
       integer*4     gbl_lotid_no
       INTEGER       lotId_len
       INTEGER       mcsLoc_len
       CHARACTER*(TYP__LOTIDSZ)  lotIdStr
       CHARACTER*(TYP__MCSLOCSZ)  mcsLocationStr
       INTEGER i

       WRITE (lotIdStr,'(50a)') (lotId(i),i=1,lotId_len)
       WRITE (mcsLocationStr,'(50a)') (mcsLocation(i),i=1,mcsLoc_len)

       WRITE( *,*) '1:lot Id(|',lotId_len,')|',lotIdStr,
     1          '|mcs Location(|',mcsLoc_len,')|',mcsLocationStr,'|'

C*******************************************************
C    ! PROMIS initialization
C*********************************************************

C      call Cfc_Imginitlz (0)            ! no local I/O for now (0 arg)
C      call lib$establish (Ttc_TpHandler)     ! establish handler
C      call Tut_SetInput_Off ()               ! barff up if input requested

C    ! Get name of shared account and attempt to login

C       account = Ssp_Option_ServerAccount ()
C       status = Uaf_User_DetLogin (account)
C       if (.not. status) goto 1000


       call SspStat_McsSync_LockTables()
C       WRITE( *,*) 'ProLot|',wct_wipLotCom_lotId( 1 )(1:lotId_len),','
C     1                  ,wct_wipLotCom_mcsLocation(1),'|'
C      find the promis lot and update mcs location
       do gbl_lotid_no = 1, wct_wipLotCom_maxIndexUsed

         if ( wct_wipLotCom_lotId( gbl_lotid_no )(1:lotId_len) .EQ.
     1      lotIdStr ) then
           if(len(trim(mcsLocationStr)) .GT. 0) then
             WRITE( *,*) 'BFLot|',wct_wipLotCom_lotId( gbl_lotid_no ),','
     1                  ,wct_wipLotCom_mcsLocation(gbl_lotid_no),'|'

             wct_wipLotCom_mcsLocation(gbl_lotid_no) = mcsLocationStr

             WRITE( *,*) 'AFLot|',wct_wipLotCom_lotId( gbl_lotid_no ),','
     1                  ,wct_wipLotCom_mcsLocation(gbl_lotid_no),'|'
           end if

           exit

         end if

       end do

       call Wct_LockTable_ReleaseLocks


C     ! Come here when exiting server
C1000  continue
C      call Uaf_User_DetLogOut ()        ! log off PROMIS
C      call Lib$Do_Command ('EXIT')      ! EXIT from command file
      return

      end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!----------------------------------------------------------------------------
      subroutine getLotLocFromPromis(mcsLocation, lotId, lotId_len)

! Description: This function will update promis global section for certain lot

       implicit none

! Author: 15-July-2013    Zhou An-Ping

! Modifications:
!

! Include files:

      include 'FIL:TYPDEF.FOR'               ! sizing parameters
      include 'TTC:TTCMSG'
      include 'WCT:WCTDEF'
      include 'WCT:WIPLOTCOM'

       CHARACTER     mcsLocation*(*)
       BYTE          lotId(TYP__LOTIDSZ)
       INTEGER       lotId_len

       integer*4     gbl_lotid_no

       CHARACTER*(TYP__LOTIDSZ)  lotIdStr

       INTEGER i

       WRITE (lotIdStr,'(50a)') (lotId(i),i=1,lotId_len)
C       WRITE (mcsLocationStr,'(50a)') (mcsLocation(i),i=1,mcsLoc_len)

       WRITE( *,*) '1:lot Id(|',lotId_len,')|',lotIdStr,'|'
C    1          '|mcs Location(|',mcsLoc_len,')|',mcsLocationStr,'|'


       call SspStat_McsSync_LockTables()

C      find the promis lot and update mcs location
       do gbl_lotid_no = 1, wct_wipLotCom_maxIndexUsed

         if ( wct_wipLotCom_lotId( gbl_lotid_no )(1:lotId_len) .EQ.
     1      lotIdStr ) then

           WRITE( *,*) 'BFLot|',wct_wipLotCom_lotId( gbl_lotid_no ),','
     1                  ,wct_wipLotCom_mcsLocation(gbl_lotid_no),'|'

           mcsLocation = wct_wipLotCom_mcsLocation(gbl_lotid_no)

           WRITE( *,*) 'AFLot|',wct_wipLotCom_lotId( gbl_lotid_no ),','
     1                  ,wct_wipLotCom_mcsLocation(gbl_lotid_no),'|'
           exit

         end if

       end do

       call Wct_LockTable_ReleaseLocks

      return

      end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      subroutine SspStat_McsSync_LockTables

! Description: This function will lock the WIP tables

       implicit none

! Author: 08-Feb-1996    Sebastien Spicer

! Modifications:
!

! Include files:

C    ! Routines used
       logical*4 Wct_LockTable_ReadLock
C    !subroutine Wct_LockTable_ReleaseLocks

C    ! Local variables
       logical*4 gotLock

C    ! Code

C      ! Get lock on workcenter table.
       gotLock = .false.
       do while (.not. gotLock)
        gotLock = Wct_LockTable_ReadLock()
        if (gotLock) then
C         ! no deadlock, lock obtained successfully.
        else
C         ! deadlock detected. Release all locks and try again.
         call Wct_LockTable_ReleaseLocks
        end if
       end do

       return

      end
! GF - End of CIM System custom modifications
