metadataFolder="../nu4100/app/common/infrastructure/metadata"
sensorSyncFolder="../nu4100/app/common/infrastructure/sensorsync"
python3 -m venv venv/ # Create virtual environment
venv/bin/pip3 install protobuf==3.20.*  grpcio-tools
venv/bin/python3 $metadataFolder/nanopb/generator/nanopb_generator.py $metadataFolder/*.proto -I $metadataFolder -D $metadataFolder/src
venv/bin/python3 $metadataFolder/nanopb/generator/nanopb_generator.py $sensorSyncFolder/*.proto -I $sensorSyncFolder -I $metadataFolder -D $sensorSyncFolder/src
protoc $metadataFolder/*.proto -I $metadataFolder --python_out=../../scripts
protoc $sensorSyncFolder/*.proto -I $sensorSyncFolder -I $metadataFolder --python_out=../../scripts
mv $metadataFolder/src/metadata.pb.h ../nu4100/app/api/common/metadata.pb.h
mv $sensorSyncFolder/src/sensor_sync.pb.h ../nu4100/app/api/common/sensor_sync.pb.h