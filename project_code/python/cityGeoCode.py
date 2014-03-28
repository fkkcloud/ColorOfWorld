'''
Author : Jae Hyun Yoo
Date : Mar 25 2014
Description : getting geocode experimental
'''
#------------------------------------------------
import json
import urllib
from urllib2 import urlopen


#----------------------------------------------
def drange( min, max, sample):
    ls = []
    min = min * 1000000
    max = max * 1000000
    sample = sample * 1000000
    min = int(min)
    max = int(max)
    sample = int(sample)
    i = min
    while i < max:
        ls.append(i * 0.000001)
        i = i + sample
    return ls
        

#------------------------------------------------
# debug location
#latitude = 37.568260
#longitude = 126.97783

#earth
latitudes = drange(37.734612, 37.826057, 0.0004)
longitudes = drange(-122.508373, -122.377739, 0.0005)

#debug locations
#latitudes = range(37, 40)
#longitudes = range(126, 129)


#------------------------------------------------
# get Json from location given
def getJson( latitude, longitude, type ):
    if type == 'mapquest':
        urlPath = "http://open.mapquestapi.com/geocoding/v1/reverse?key=Fmjtd%7Cluur2l082u%2Cbw%3Do5-9a709z&location=" + str(latitude) + "," + str(longitude)
    elif type == 'google':
            urlPath = "http://maps.googleapis.com/maps/api/geocode/json?latlng=" + str( latitude ) + "," + str( longitude ) + "&sensor=true"
    jsonBuffer = urlopen(urlPath)
    jsonObj = json.loads(jsonBuffer.read())
    return jsonObj


#------------------------------------------------
# get city name and count
def getGeoCode(type):
    
    file = open('geocode_demo2', 'w')
    print len(latitudes)
    print len(longitudes)
    print len(latitudes) * len(longitudes)
    i = 0;
    mapData = {}
    for latitude in latitudes:
        for longitude in longitudes:
            bufferJson = getJson( latitude, longitude, type )

            try:
                if type == 'mapquest':
                    bff = bufferJson['results'][0]['locations'][0]
                    city_name = bff['street'] + ',' + bff['adminArea4'] + ',' + bff['adminArea5'] 

                elif type == 'google':
                    city_name = bufferJson['results'][0]['formatted_address'].split(',')[-1].strip()    
                    
            # if not found - set to ocean (google)
            except IndexError:
                city_name = 'Unknown486'
            except UnicodeEncodeError:
                city_name = 'UnicideEncodeError'
            
            # if not found - set to ocean (mapquest)
            if city_name == '':
                city_name = 'Unknown486'

            # debug print city name
            i += 1
            mapKey = (latitude, longitude)

            mapData[mapKey] = city_name

            try:
                bufferWrite = str(mapKey[0]) + ',' + str(mapKey[1]) + ',' + mapData[mapKey] + '\n'
                print bufferWrite
                file.write(bufferWrite)
            except:
                bufferWrite = str(mapKey[0]) + ',' + str(mapKey[1]) + ',' + 'Red,Green,Blue' + '\n'
                file.write(bufferWrite)
    file.close()
    return mapData


#------------------------------------------------
def main():
    maps = getGeoCode('mapquest')
    print "Ran"


#------------------------------------------------
if __name__ == "__main__":
    main()

