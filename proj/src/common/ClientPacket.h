//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <vector>
#include <inet/common/packet/chunk/FieldsChunk.h>

#ifndef SRC_TUMCLIENT_CLIENTPACKET_H_
#define SRC_TUMCLIENT_CLIENTPACKET_H_

using namespace inet;
using namespace std;

class ClientPacket : public FieldsChunk {
private:
    vector<int> tracks;
public:
    ClientPacket();
    virtual ~ClientPacket();
    void setTracks(const vector<int> &tracks);
    const vector<int>& getTracks() const;
};

#endif /* SRC_TUMCLIENT_CLIENTPACKET_H_ */
