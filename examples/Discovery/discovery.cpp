/*
* This program is free software; you can use it, redistribute it
 * and / or modify it under the terms of the GNU General Public License
 * (GPL) as published by the Free Software Foundation; either version 3
 * of the License or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, in a file called gpl.txt or license.txt.
 * If not, write to the Free Software Foundation Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307 USA
 */

#include <Arduino.h>
#include <BLESerial.h>

BLESerial client;
NimBLEAdvertisedDevice *device;
uint8_t run = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    if (client.begin("BLESerial")) {
        Serial.print("Discover BLE devices...");
        BLEScanResultsSet *bleDeviceList = client.discover();
        Serial.println("...done.");

        if (bleDeviceList != nullptr && bleDeviceList->getCount() > 0) {
            device = bleDeviceList->getDevice(0);
        }

        while (!client.connected()) {
            if (client.connect(device->getAddress())) {
                Serial.printf("Connected to %s.\n", device->getAddress().toString().c_str());
            }
        }
    }
}

void loop() {
    if (client.connected()) {
        client.print("AT D\r");
        delay(100);

        client.print("AT Z\r");
        delay(100);

        client.print("AT @1\r");

        const uint32_t startTime = millis();
        while (millis() - startTime < 10000) {
            if (client.available()) {
                const char recChar = client.read();
                if (recChar != '>') {
                    Serial.write(recChar);
                } else {
                    Serial.println();
                    break;
                }
            }
        }
        ++run;

        if (run > 10 && client.disconnect()) {
            Serial.printf("Disconnected from %s.\n", device->getAddress().toString().c_str());
        }
    }
}
