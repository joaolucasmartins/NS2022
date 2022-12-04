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
// //
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
//

package tum.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;
import inet.node.inet.Router;

import tum.tumclient.TumClient;
import tum.tumtrain.TumTrain;
import tum.tumserver.TumServer;

import tum.sbahn.SbahnStop;

network MuenchenNetwork
{
    parameters:
        int nClients;
        int nTrains;
    types:
        channel C extends DatarateChannel
        {
            datarate = 100Mbps;
            delay = 0.1us;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s;");
        }
        client[nClients]: TumClient {
            parameters:
                @display("i=device/laptop;p=250,100,col,100;");
        }
        train[nTrains]: TumTrain {
            parameters:
                @display("i=trains/s2;p=550,100,col,100");
        }
        server: TumServer {
        	parameters:
        	    @display("p=400,250;i=device/pc2");
        }
        clientRouter: Router {
        	parameters:
        	    @display("p=375,150");
        }
        trainRouter: Router {
        	parameters:
        	    @display("p=425,150");
        }


		stop_972: SbahnStop { parameters: x="74874"; y="5988"; stopName="Flughafen Terminal"; }
        stop_13903: SbahnStop { parameters: x="72672"; y="6153"; stopName="Flughafen Besucherpark"; }
        stop_9080: SbahnStop { parameters: x="62406"; y="9186"; stopName="Neufahrn(b Freising)"; }
        stop_1576: SbahnStop { parameters: x="57908"; y="11075"; stopName="Eching"; }
        stop_3263: SbahnStop { parameters: x="54376"; y="12546"; stopName="Lohhof"; }
        stop_637: SbahnStop { parameters: x="53129"; y="13971"; stopName="Unterschleissheim"; }
        stop_3566: SbahnStop { parameters: x="52048"; y="15625"; stopName="Oberschleissheim"; }
        stop_14506: SbahnStop { parameters: x="50341"; y="19999"; stopName="Feldmoching"; }
        stop_5870: SbahnStop { parameters: x="48673"; y="21669"; stopName="Fasanerie"; }
        stop_7544: SbahnStop { parameters: x="46831"; y="23317"; stopName="Moosach"; }
        stop_9374: SbahnStop { parameters: x="46546"; y="26978"; stopName="Laim"; }
        stop_11586: SbahnStop { parameters: x="48177"; y="27017"; stopName="Hirschgarten"; }
        stop_10734: SbahnStop { parameters: x="49954"; y="27098"; stopName="Donnersbergerbruecke"; }
        stop_12960: SbahnStop { parameters: x="51096"; y="27056"; stopName="Hackerbruecke"; }
        stop_7640: SbahnStop { parameters: x="52038"; y="27230"; stopName="Hbf"; }
        stop_13692: SbahnStop { parameters: x="52826"; y="27421"; stopName="Karlsplatz"; }
        stop_13337: SbahnStop { parameters: x="53793"; y="27660"; stopName="Marienplatz"; }
        stop_5508: SbahnStop { parameters: x="54568"; y="27942"; stopName="Isartor"; }
        stop_8257: SbahnStop { parameters: x="55550"; y="28439"; stopName="Rosenheimer Platz"; }
        stop_1123: SbahnStop { parameters: x="56661"; y="28579"; stopName="Ostbahnhof"; }
        stop_13736: SbahnStop { parameters: x="57852"; y="27940"; stopName="Leuchtenbergring"; }
        stop_7742: SbahnStop { parameters: x="66601"; y="4892"; stopName="Pulling(b Freising)"; }
        stop_4181: SbahnStop { parameters: x="70697"; y="1859"; stopName="Freising"; }
        stop_7803: SbahnStop { parameters: x="55885"; y="30274"; stopName="Giesing"; }
        stop_7897: SbahnStop { parameters: x="55829"; y="29525"; stopName="St.Martin-Str."; }
        stop_5266: SbahnStop { parameters: x="87461"; y="11027"; stopName="Erding"; }
        stop_9449: SbahnStop { parameters: x="86380"; y="12066"; stopName="Altenerding"; }
        stop_7663: SbahnStop { parameters: x="85638"; y="13857"; stopName="Aufhausen(b Erding)"; }
        stop_5404: SbahnStop { parameters: x="84552"; y="17013"; stopName="St Koloman"; }
        stop_12264: SbahnStop { parameters: x="84186"; y="19552"; stopName="Ottenhofen"; }
        stop_1754: SbahnStop { parameters: x="82472"; y="22006"; stopName="Markt Schwaben"; }
        stop_628: SbahnStop { parameters: x="77234"; y="24226"; stopName="Poing"; }
        stop_7962: SbahnStop { parameters: x="74504"; y="24914"; stopName="Grub"; }
        stop_6678: SbahnStop { parameters: x="72180"; y="25499"; stopName="Heimstetten"; }
        stop_12501: SbahnStop { parameters: x="69444"; y="26185"; stopName="Feldkirchen(b Muenchen)"; }
        stop_1018: SbahnStop { parameters: x="64048"; y="26967"; stopName="Riem"; }
        stop_10048: SbahnStop { parameters: x="59574"; y="27964"; stopName="Berg am Laim"; }
        stop_743: SbahnStop { parameters: x="44059"; y="24940"; stopName="Obermenzing"; }
        stop_10775: SbahnStop { parameters: x="43525"; y="23591"; stopName="Untermenzing"; }
        stop_9675: SbahnStop { parameters: x="43064"; y="22388"; stopName="Allach"; }
        stop_10423: SbahnStop { parameters: x="42205"; y="20231"; stopName="Karlsfeld"; }
        stop_12642: SbahnStop { parameters: x="40600"; y="15914"; stopName="Dachau Bahnhof"; }
        stop_10915: SbahnStop { parameters: x="40235"; y="14704"; stopName="Dachau Stadt"; }
        stop_9748: SbahnStop { parameters: x="34216"; y="13380"; stopName="Bachern"; }
        stop_11532: SbahnStop { parameters: x="32718"; y="11258"; stopName="Schwabhausen"; }
        stop_10961: SbahnStop { parameters: x="35136"; y="9164"; stopName="Niederroth"; }
        stop_11690: SbahnStop { parameters: x="34117"; y="6113"; stopName="Markt Indersdorf"; }
        stop_9299: SbahnStop { parameters: x="30666"; y="7232"; stopName="Arnbach"; }
        stop_13685: SbahnStop { parameters: x="26642"; y="8028"; stopName="Erdweg"; }
        stop_10086: SbahnStop { parameters: x="23752"; y="5908"; stopName="Kleinberghofen"; }
        stop_2477: SbahnStop { parameters: x="21728"; y="2843"; stopName="Altomuenster"; }
        stop_557: SbahnStop { parameters: x="43260"; y="0"; stopName="Petershausen(Obb)"; }
        stop_1747: SbahnStop { parameters: x="40768"; y="5243"; stopName="Vierkirchen-Esterhofen"; }
        stop_9140: SbahnStop { parameters: x="40721"; y="7994"; stopName="Roehrmoos"; }
        stop_9408: SbahnStop { parameters: x="41736"; y="12308"; stopName="Hebertshausen"; }
        stop_7036: SbahnStop { parameters: x="42253"; y="26442"; stopName="Pasing"; }
        stop_4618: SbahnStop { parameters: x="49427"; y="28029"; stopName="Heimeranplatz"; }
        stop_1630: SbahnStop { parameters: x="49891"; y="30573"; stopName="Mittersendling"; }
        stop_3026: SbahnStop { parameters: x="49524"; y="31874"; stopName="Siemenswerke"; }
        stop_9954: SbahnStop { parameters: x="48950"; y="33355"; stopName="Solln"; }
        stop_13764: SbahnStop { parameters: x="49341"; y="34260"; stopName="Grosshesselohe Isartalbf"; }
        stop_4735: SbahnStop { parameters: x="48432"; y="35476"; stopName="Pullach"; }
        stop_3463: SbahnStop { parameters: x="47218"; y="37033"; stopName="Hoellriegelskreuth"; }
        stop_2932: SbahnStop { parameters: x="11599"; y="28150"; stopName="Grafrath"; }
        stop_10962: SbahnStop { parameters: x="15400"; y="26724"; stopName="Schoengeising"; }
        stop_4086: SbahnStop { parameters: x="18762"; y="25277"; stopName="Buchenau"; }
        stop_5374: SbahnStop { parameters: x="22550"; y="24158"; stopName="Fuerstenfeldbruck"; }
        stop_3104: SbahnStop { parameters: x="29113"; y="23443"; stopName="Eichenau"; }
        stop_3784: SbahnStop { parameters: x="31588"; y="24159"; stopName="Puchheim"; }
        stop_2755: SbahnStop { parameters: x="37538"; y="25765"; stopName="Aubing"; }
        stop_12468: SbahnStop { parameters: x="39079"; y="25908"; stopName="Leienfelsstr."; }
        stop_7849: SbahnStop { parameters: x="0"; y="30766"; stopName="Geltendorf"; }
        stop_14447: SbahnStop { parameters: x="3905"; y="30905"; stopName="Tuerkenfeld"; }
        stop_9955: SbahnStop { parameters: x="54622"; y="39418"; stopName="Deisenhofen"; }
        stop_1906: SbahnStop { parameters: x="55612"; y="37789"; stopName="Furth(b Deisenhofen)"; }
        stop_9286: SbahnStop { parameters: x="57168"; y="36232"; stopName="Taufkirchen"; }
        stop_3630: SbahnStop { parameters: x="57490"; y="34874"; stopName="Unterhaching"; }
        stop_1615: SbahnStop { parameters: x="57191"; y="33376"; stopName="Fasanenpark"; }
        stop_5272: SbahnStop { parameters: x="56790"; y="32072"; stopName="Fasangarten"; }
        stop_6404: SbahnStop { parameters: x="39502"; y="25057"; stopName="Langwied"; }
        stop_6024: SbahnStop { parameters: x="37030"; y="23700"; stopName="Lochhausen"; }
        stop_6290: SbahnStop { parameters: x="33637"; y="21859"; stopName="Groebenzell"; }
        stop_3996: SbahnStop { parameters: x="29806"; y="20364"; stopName="Olching"; }
        stop_411: SbahnStop { parameters: x="27972"; y="20146"; stopName="Esting"; }
        stop_1375: SbahnStop { parameters: x="26253"; y="20058"; stopName="Gernlinden"; }
        stop_2265: SbahnStop { parameters: x="21924"; y="19951"; stopName="Maisach"; }
        stop_4278: SbahnStop { parameters: x="18180"; y="19788"; stopName="Malching"; }
        stop_2790: SbahnStop { parameters: x="13764"; y="19684"; stopName="Mammendorf"; }
        stop_13192: SbahnStop { parameters: x="61718"; y="43947"; stopName="Sauerlach"; }
        stop_13839: SbahnStop { parameters: x="64467"; y="50226"; stopName="Otterfing"; }
        stop_1050: SbahnStop { parameters: x="65978"; y="52927"; stopName="Holzkirchen"; }
        stop_14593: SbahnStop { parameters: x="62594"; y="28789"; stopName="Trudering"; }
        stop_8038: SbahnStop { parameters: x="66110"; y="29524"; stopName="Gronsdorf"; }
        stop_10996: SbahnStop { parameters: x="69358"; y="30220"; stopName="Haar"; }
        stop_13836: SbahnStop { parameters: x="72664"; y="30947"; stopName="Vaterstetten"; }
        stop_13704: SbahnStop { parameters: x="74947"; y="31443"; stopName="Baldham"; }
        stop_13037: SbahnStop { parameters: x="79514"; y="32407"; stopName="Zorneding"; }
        stop_610: SbahnStop { parameters: x="82662"; y="33123"; stopName="Eglharting"; }
        stop_11661: SbahnStop { parameters: x="84982"; y="34270"; stopName="Kirchseeon"; }
        stop_6426: SbahnStop { parameters: x="90293"; y="37035"; stopName="Grafing Bahnhof"; }
        stop_4852: SbahnStop { parameters: x="93254"; y="33866"; stopName="Ebersberg"; }
        stop_14716: SbahnStop { parameters: x="92540"; y="36628"; stopName="Grafing Stadt"; }
        stop_993: SbahnStop { parameters: x="23568"; y="50632"; stopName="Tutzing"; }
        stop_1385: SbahnStop { parameters: x="25263"; y="46670"; stopName="Feldafing"; }
        stop_197: SbahnStop { parameters: x="26909"; y="45163"; stopName="Possenhofen"; }
        stop_4950: SbahnStop { parameters: x="30587"; y="41772"; stopName="Starnberg"; }
        stop_2487: SbahnStop { parameters: x="30992"; y="40782"; stopName="Starnberg Nord"; }
        stop_4036: SbahnStop { parameters: x="33860"; y="34311"; stopName="Gauting"; }
        stop_8480: SbahnStop { parameters: x="36332"; y="32040"; stopName="Stockdorf"; }
        stop_12313: SbahnStop { parameters: x="37618"; y="30820"; stopName="Planegg"; }
        stop_1456: SbahnStop { parameters: x="38825"; y="29369"; stopName="Graefelfing"; }
        stop_1939: SbahnStop { parameters: x="39349"; y="28518"; stopName="Lochham"; }
        stop_230: SbahnStop { parameters: x="40455"; y="26446"; stopName="Westkreuz"; }
        stop_5504: SbahnStop { parameters: x="38971"; y="49904"; stopName="Wolfratshausen"; }
        stop_5990: SbahnStop { parameters: x="39912"; y="46053"; stopName="Icking"; }
        stop_12322: SbahnStop { parameters: x="41610"; y="43500"; stopName="Ebenhausen-Schaeftlarn"; }
        stop_1646: SbahnStop { parameters: x="42018"; y="42222"; stopName="Hohenschaeftlarn"; }
        stop_561: SbahnStop { parameters: x="44605"; y="39467"; stopName="Baierbrunn"; }
        stop_6295: SbahnStop { parameters: x="46030"; y="38168"; stopName="Buchenhain"; }
        stop_4140: SbahnStop { parameters: x="49876"; y="29640"; stopName="Harras"; }
        stop_491: SbahnStop { parameters: x="59384"; y="32025"; stopName="Perlach"; }
        stop_5485: SbahnStop { parameters: x="60764"; y="32490"; stopName="Neuperlach Sued"; }
        stop_3812: SbahnStop { parameters: x="62299"; y="33795"; stopName="Neubiberg"; }
        stop_11238: SbahnStop { parameters: x="64045"; y="35002"; stopName="Ottobrunn"; }
        stop_12969: SbahnStop { parameters: x="66127"; y="36694"; stopName="Hohenbrunn"; }
        stop_13523: SbahnStop { parameters: x="67557"; y="37836"; stopName="Waechterhof"; }
        stop_6224: SbahnStop { parameters: x="68178"; y="39535"; stopName="Hoehenkirchen-Siegertsbrunn"; }
        stop_2898: SbahnStop { parameters: x="69804"; y="42031"; stopName="Duerrnhaar"; }
        stop_7283: SbahnStop { parameters: x="73201"; y="44480"; stopName="Aying"; }
        stop_10217: SbahnStop { parameters: x="73916"; y="45360"; stopName="Peiss"; }
        stop_9045: SbahnStop { parameters: x="74445"; y="46835"; stopName="Grosshelfendorf"; }
        stop_5163: SbahnStop { parameters: x="72156"; y="49716"; stopName="Kreuzstrasse"; }
        stop_11594: SbahnStop { parameters: x="67692"; y="10435"; stopName="Hallbergmoos"; }
        stop_8148: SbahnStop { parameters: x="64205"; y="18758"; stopName="Ismaning"; }
        stop_3167: SbahnStop { parameters: x="60942"; y="22338"; stopName="Unterfoehring"; }
        stop_3107: SbahnStop { parameters: x="60963"; y="24487"; stopName="Johanneskirchen"; }
        stop_7966: SbahnStop { parameters: x="61086"; y="25661"; stopName="Englschalking"; }
        stop_3681: SbahnStop { parameters: x="61183"; y="26382"; stopName="Daglfing"; }
        stop_8416: SbahnStop { parameters: x="38602"; y="27008"; stopName="Neuaubing"; }
        stop_6256: SbahnStop { parameters: x="37296"; y="27360"; stopName="Freiham"; }
        stop_2269: SbahnStop { parameters: x="35051"; y="28009"; stopName="Harthaus"; }
        stop_5097: SbahnStop { parameters: x="32641"; y="28468"; stopName="Germering-Unterpfaffenhofen"; }
        stop_3260: SbahnStop { parameters: x="28702"; y="30441"; stopName="Geisenbrunn"; }
        stop_1448: SbahnStop { parameters: x="26256"; y="30776"; stopName="Gilching-Argelsried"; }
        stop_7294: SbahnStop { parameters: x="25149"; y="31451"; stopName="Neugilching"; }
        stop_3690: SbahnStop { parameters: x="21408"; y="33631"; stopName="Wessling"; }
        stop_14065: SbahnStop { parameters: x="16924"; y="35260"; stopName="Steinebach"; }
        stop_12951: SbahnStop { parameters: x="16118"; y="37232"; stopName="Seefeld-Hechendorf"; }
        stop_4187: SbahnStop { parameters: x="13311"; y="41492"; stopName="Herrsching"; }
		
		
    connections:
        for i=0..nClients-1 {
            client[i].pppg++ <--> C <--> clientRouter.pppg++;
        }
        for i=0..nTrains-1 {
            train[i].pppg++ <--> C <--> trainRouter.pppg++;
        }
        clientRouter.pppg++ <--> C <--> server.pppg++;
        trainRouter.pppg++ <--> C <--> server.pppg++;
        
		stop_972.port++ <--> stop_13903.port++;
        stop_13903.port++ <--> stop_11594.port++;
        stop_13903.port++ <--> stop_9080.port++;
        stop_9080.port++ <--> stop_1576.port++;
        stop_9080.port++ <--> stop_7742.port++;
        stop_1576.port++ <--> stop_3263.port++;
        stop_3263.port++ <--> stop_637.port++;
        stop_637.port++ <--> stop_3566.port++;
        stop_3566.port++ <--> stop_14506.port++;
        stop_14506.port++ <--> stop_5870.port++;
        stop_5870.port++ <--> stop_7544.port++;
        stop_7544.port++ <--> stop_9374.port++;
        stop_9374.port++ <--> stop_11586.port++;
        stop_9374.port++ <--> stop_7036.port++;
        stop_9374.port++ <--> stop_743.port++;
        stop_11586.port++ <--> stop_10734.port++;
        stop_10734.port++ <--> stop_12960.port++;
        stop_10734.port++ <--> stop_4618.port++;
        stop_12960.port++ <--> stop_7640.port++;
        stop_7640.port++ <--> stop_13692.port++;
        stop_13692.port++ <--> stop_13337.port++;
        stop_13337.port++ <--> stop_5508.port++;
        stop_5508.port++ <--> stop_8257.port++;
        stop_8257.port++ <--> stop_1123.port++;
        stop_1123.port++ <--> stop_13736.port++;
        stop_1123.port++ <--> stop_7897.port++;
        stop_13736.port++ <--> stop_10048.port++;
        stop_13736.port++ <--> stop_3681.port++;
        stop_7742.port++ <--> stop_4181.port++;
        stop_7803.port++ <--> stop_5272.port++;
        stop_7803.port++ <--> stop_7897.port++;
        stop_7803.port++ <--> stop_491.port++;
        stop_5266.port++ <--> stop_9449.port++;
        stop_9449.port++ <--> stop_7663.port++;
        stop_7663.port++ <--> stop_5404.port++;
        stop_5404.port++ <--> stop_12264.port++;
        stop_12264.port++ <--> stop_1754.port++;
        stop_1754.port++ <--> stop_628.port++;
        stop_628.port++ <--> stop_7962.port++;
        stop_7962.port++ <--> stop_6678.port++;
        stop_6678.port++ <--> stop_12501.port++;
        stop_12501.port++ <--> stop_1018.port++;
        stop_1018.port++ <--> stop_10048.port++;
        stop_10048.port++ <--> stop_14593.port++;
        stop_743.port++ <--> stop_10775.port++;
        stop_10775.port++ <--> stop_9675.port++;
        stop_9675.port++ <--> stop_10423.port++;
        stop_10423.port++ <--> stop_12642.port++;
        stop_12642.port++ <--> stop_9408.port++;
        stop_12642.port++ <--> stop_10915.port++;
        stop_10915.port++ <--> stop_9748.port++;
        stop_9748.port++ <--> stop_11532.port++;
        stop_11532.port++ <--> stop_10961.port++;
        stop_10961.port++ <--> stop_11690.port++;
        stop_11690.port++ <--> stop_9299.port++;
        stop_9299.port++ <--> stop_13685.port++;
        stop_13685.port++ <--> stop_10086.port++;
        stop_10086.port++ <--> stop_2477.port++;
        stop_557.port++ <--> stop_1747.port++;
        stop_1747.port++ <--> stop_9140.port++;
        stop_9140.port++ <--> stop_9408.port++;
        stop_7036.port++ <--> stop_12468.port++;
        stop_7036.port++ <--> stop_6404.port++;
        stop_7036.port++ <--> stop_230.port++;
        stop_4618.port++ <--> stop_4140.port++;
        stop_1630.port++ <--> stop_3026.port++;
        stop_1630.port++ <--> stop_4140.port++;
        stop_3026.port++ <--> stop_9954.port++;
        stop_9954.port++ <--> stop_13764.port++;
        stop_13764.port++ <--> stop_4735.port++;
        stop_4735.port++ <--> stop_3463.port++;
        stop_3463.port++ <--> stop_6295.port++;
        stop_2932.port++ <--> stop_14447.port++;
        stop_2932.port++ <--> stop_10962.port++;
        stop_10962.port++ <--> stop_4086.port++;
        stop_4086.port++ <--> stop_5374.port++;
        stop_5374.port++ <--> stop_3104.port++;
        stop_3104.port++ <--> stop_3784.port++;
        stop_3784.port++ <--> stop_2755.port++;
        stop_2755.port++ <--> stop_12468.port++;
        stop_7849.port++ <--> stop_14447.port++;
        stop_9955.port++ <--> stop_13192.port++;
        stop_9955.port++ <--> stop_1906.port++;
        stop_1906.port++ <--> stop_9286.port++;
        stop_9286.port++ <--> stop_3630.port++;
        stop_3630.port++ <--> stop_1615.port++;
        stop_1615.port++ <--> stop_5272.port++;
        stop_6404.port++ <--> stop_6024.port++;
        stop_6024.port++ <--> stop_6290.port++;
        stop_6290.port++ <--> stop_3996.port++;
        stop_3996.port++ <--> stop_411.port++;
        stop_411.port++ <--> stop_1375.port++;
        stop_1375.port++ <--> stop_2265.port++;
        stop_2265.port++ <--> stop_4278.port++;
        stop_4278.port++ <--> stop_2790.port++;
        stop_13192.port++ <--> stop_13839.port++;
        stop_13839.port++ <--> stop_1050.port++;
        stop_14593.port++ <--> stop_8038.port++;
        stop_8038.port++ <--> stop_10996.port++;
        stop_10996.port++ <--> stop_13836.port++;
        stop_13836.port++ <--> stop_13704.port++;
        stop_13704.port++ <--> stop_13037.port++;
        stop_13037.port++ <--> stop_610.port++;
        stop_610.port++ <--> stop_11661.port++;
        stop_11661.port++ <--> stop_6426.port++;
        stop_6426.port++ <--> stop_14716.port++;
        stop_4852.port++ <--> stop_14716.port++;
        stop_993.port++ <--> stop_1385.port++;
        stop_1385.port++ <--> stop_197.port++;
        stop_197.port++ <--> stop_4950.port++;
        stop_4950.port++ <--> stop_2487.port++;
        stop_2487.port++ <--> stop_4036.port++;
        stop_4036.port++ <--> stop_8480.port++;
        stop_8480.port++ <--> stop_12313.port++;
        stop_12313.port++ <--> stop_1456.port++;
        stop_1456.port++ <--> stop_1939.port++;
        stop_1939.port++ <--> stop_230.port++;
        stop_230.port++ <--> stop_8416.port++;
        stop_5504.port++ <--> stop_5990.port++;
        stop_5990.port++ <--> stop_12322.port++;
        stop_12322.port++ <--> stop_1646.port++;
        stop_1646.port++ <--> stop_561.port++;
        stop_561.port++ <--> stop_6295.port++;
        stop_491.port++ <--> stop_5485.port++;
        stop_5485.port++ <--> stop_3812.port++;
        stop_3812.port++ <--> stop_11238.port++;
        stop_11238.port++ <--> stop_12969.port++;
        stop_12969.port++ <--> stop_13523.port++;
        stop_13523.port++ <--> stop_6224.port++;
        stop_6224.port++ <--> stop_2898.port++;
        stop_2898.port++ <--> stop_7283.port++;
        stop_7283.port++ <--> stop_10217.port++;
        stop_10217.port++ <--> stop_9045.port++;
        stop_9045.port++ <--> stop_5163.port++;
        stop_11594.port++ <--> stop_8148.port++;
        stop_8148.port++ <--> stop_3167.port++;
        stop_3167.port++ <--> stop_3107.port++;
        stop_3107.port++ <--> stop_7966.port++;
        stop_7966.port++ <--> stop_3681.port++;
        stop_8416.port++ <--> stop_6256.port++;
        stop_6256.port++ <--> stop_2269.port++;
        stop_2269.port++ <--> stop_5097.port++;
        stop_5097.port++ <--> stop_3260.port++;
        stop_3260.port++ <--> stop_1448.port++;
        stop_1448.port++ <--> stop_7294.port++;
        stop_7294.port++ <--> stop_3690.port++;
        stop_3690.port++ <--> stop_14065.port++;
        stop_14065.port++ <--> stop_12951.port++;
        stop_12951.port++ <--> stop_4187.port++;
}
