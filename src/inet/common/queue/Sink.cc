//
// Copyright (C) 1992-2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/common/queue/Sink.h"

namespace inet {

Define_Module(Sink);

simsignal_t Sink::rcvdPkSignal = registerSignal("rcvdPk");

void Sink::initialize()
{
    numPackets = 0;
    numBits = 0;
    Bits = 0;
    throughput = 0;
    packetPerSec = 0;
    interval = 0.5;
    TH = 0;
    msg_ap = new cMessage("Ap_selfMsg");
    scheduleAt(simTime()+interval,msg_ap);
    stat_throughput = registerSignal("th");

    WATCH(numPackets);
    WATCH(numBits);
    WATCH(throughput);
    WATCH(packetPerSec);
}

void Sink::handleMessage(cMessage *msg)
{
    if(msg==msg_ap)
    {
        TH = Bits/interval;
        emit(stat_throughput,TH);
        cancelEvent(msg_ap);
        scheduleAt(simTime()+interval,msg_ap);
        Bits=0;
    }
    else
    {
        numPackets++;
        cPacket *packet = PK(msg);
        Bits += packet->getBitLength();
        numBits += packet->getBitLength();
        emit(rcvdPkSignal, packet);
        throughput = numBits / simTime();
        packetPerSec = numPackets / simTime();
        delete msg;
    }
}

void Sink::finish()
{
    recordScalar("throughput", throughput);
    recordScalar("packetPerSec", packetPerSec);
}

} // namespace inet

