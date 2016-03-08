/***********************************************************************
*
* Copyright (c) 2015-2016 Barbara Geller
* Copyright (c) 2015-2016 Ansel Sermersheim
* All rights reserved.
*
* This file is part of libCsSignal
*
* libCsSignal is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* version 2.1 as published by the Free Software Foundation.
*
* libCsSignal is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with CopperSpice.  If not, see
* <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#ifndef LIB_CS_SLOT_H
#define LIB_CS_SLOT_H

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace CsSignal {

class SignalBase;
enum class ConnectionType;

class PendingSlot
{
   public:
      PendingSlot(const PendingSlot &) = delete;
      PendingSlot(PendingSlot &&) = default;

      PendingSlot(SignalBase *sender, std::unique_ptr<Internal::BentoAbstract> signal_Bento, SlotBase *receiver,
                     std::unique_ptr<Internal::BentoAbstract>  slot_Bento,
                     std::unique_ptr<Internal::TeaCupAbstract> teaCup_Data);

      SignalBase *sender() const {
         return m_sender;
      }

      SlotBase *receiver() const {
         return m_receiver;
      }

      void operator()() const;

   private:
      SignalBase *m_sender;
      std::unique_ptr<Internal::BentoAbstract>  m_signal_Bento;
      SlotBase *m_receiver;
      std::unique_ptr<Internal::BentoAbstract>  m_slot_Bento;
      std::unique_ptr<Internal::TeaCupAbstract> m_teaCup_Data;
};

class SlotBase
{
   public:
      SlotBase();
      SlotBase(const SlotBase &);
      virtual ~SlotBase();

      // SlotBase(SlotBase &&);
      // operator=(const SlotBase &);
      // operator=(SlotBase &&);
     
      SignalBase *sender() const;

   private:
      static thread_local SignalBase *threadLocal_currentSender;

      // list of possible Senders for this Receiver
      mutable std::vector<const SignalBase *> m_possibleSenders;

      mutable std::mutex m_mutex_possibleSenders;

      virtual bool compareThreads() const;
      virtual void queueSlot(PendingSlot data, ConnectionType type);

      friend class SignalBase;

      template<class Sender, class SignalClass, class ...SignalArgTypes, class ...Ts>
      friend void activate(Sender &sender, void (SignalClass::*signal)(SignalArgTypes...), Ts &&... Vs);

      template<class Sender, class Receiver>
      friend bool internal_disconnect(const Sender &sender, const Internal::BentoAbstract *signalBento, 
                  const Receiver *receiver, const Internal::BentoAbstract *slotBento); 
};


}

#endif