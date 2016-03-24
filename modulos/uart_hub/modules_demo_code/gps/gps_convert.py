"""
$GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C

GPS: 
[N,S] => dd mm.mmm => 39 39.53990 N
[W,E] => ddd mm.mmm => 008 22.03619 W

[ N/S ](mm.mmmm / 60) + dd
[ W/E ](mm.mmmm / 60) + ddd

[ S ] => ((mm.mmmm / 60) + dd) * -1
[ W ] => ((mm.mmmm / 60) + ddd) * -1

CALC:

(mm.mmmm/60) => (39.53990/60) = 0,658998333
(mm.mmmm/60) => (22.03619/60) = 0,367269833

[ N/S ](mm.mmmm / 60) + dd => 0,658998333 + 39 = 39,658998333
[ W/E ](mm.mmmm / 60) + ddd => 0,367269833 + 008 = 8,367269833

[ W ] => 8,367269833 * -1 = -8,367269833

Field	Meaning
0	Message ID $GPGLL
1	Latitude in dd mm,mmmm format (0-7 decimal places)
2	Direction of latitude N: North S: South
3	Longitude in ddd mm,mmmm format (0-7 decimal places)
4	Direction of longitude E: East W: West
5	UTC of position in hhmmss.ss format
6	Fixed text "A" shows that data is valid
7	The checksum data, always begins with *
"""
def convert_coordenates(gps_text):
	gps_parts = gps_text.split(',')

	if len(gps_parts[1]) is 0 or len(gps_parts[3]) is 0:
		return ","

	dd_lat = float(gps_parts[1][:2])
	lat = float(gps_parts[1][2:]) if len(gps_parts[1]) != 0 else 0
	dir_lat = gps_parts[2]

	ddd_lon = float(gps_parts[3][:3])
	lon = float(gps_parts[3][3:]) if len(gps_parts[3]) != 0 else 0
	dir_lon = gps_parts[4]

	latitude = (lat / 60) + dd_lat
	longitude = (lon / 60) + ddd_lon

	if dir_lat == "S":
		latitude *= -1

	if dir_lon == "W":
		longitude *= -1

	return str(latitude) + "," + str(longitude)

def main():
	# $GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C
	# $GPGLL,,,,,122202.00,A,D*7C
	print convert_coordenates('$GPGLL,,,,,122202.00,A,D*7C')

if __name__ == '__main__': main()