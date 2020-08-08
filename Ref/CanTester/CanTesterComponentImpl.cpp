// ======================================================================
// \title  CanTesterComponentImpl.cpp
// \author csommer
// \brief  cpp file for CanTester component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Ref/CanTester/CanTesterComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Logger/Logger.hpp"

#include <linux/can.h>

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CanTesterComponentImpl ::
#if FW_OBJECT_NAMES == 1
    CanTesterComponentImpl(
        const char *const compName
    ) :
      CanTesterComponentBase(compName)
#else
    CanTesterComponentImpl(void)
#endif
  {

  }

  void CanTesterComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    CanTesterComponentBase::init(queueDepth, instance);
  }

  CanTesterComponentImpl ::
    ~CanTesterComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CanTesterComponentImpl ::
    CanRecv_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // Retrieve pointer of data received
    char* msgInPtr = reinterpret_cast<char*>(fwBuffer.getdata());
    U32 size = static_cast<U32>(fwBuffer.getsize());

    // Save data in a can_frame
    struct can_frame frame;

    memcpy(&frame.can_id, &msgInPtr[0], 4);
    frame.can_dlc = msgInPtr[4];
    memcpy(&frame.data[0], &msgInPtr[8], frame.can_dlc);

    Fw::Logger::logMsg("[CANTester] ID = %08x\n", frame.can_id);
    Fw::Logger::logMsg("[CANTester] size = %d\n", frame.can_dlc);
    Fw::Logger::logMsg("[CANTester] data = ");

    for(NATIVE_UINT_TYPE i=0; i<frame.can_dlc; i++)
    {
      Fw::Logger::logMsg("%02x ", frame.data[i]);
    }
    Fw::Logger::logMsg(" END\n");
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CanTesterComponentImpl ::
    SendCanFrame_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Fw::Buffer txt;
    struct can_frame frame;
    U32 id = 0xA5A5A5A5;
    U8 data[8] = {0xDE, 0xAD, 0xC0, 0xDE, 0xDE, 0xAD, 0xBE, 0xEF}; 
    U8 dlc = 8;


    /* Copy extended frame format identifier */
    frame.can_id = ((id & CAN_EFF_MASK) | CAN_EFF_FLAG);

    /* Copy data to frame */
    for (int i = 0; i < dlc; i++)
      frame.data[i] = data[i];

    /* Set DLC */
    frame.can_dlc = dlc;

    txt.setsize(sizeof(frame));
    txt.setdata((U64)&frame);
    this->CanWrite_out(0, txt);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace Ref
