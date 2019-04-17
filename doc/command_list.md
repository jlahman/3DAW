# Command List:
## File IO:
* `import -file $filename` : add a new track with audio data from the filename
* `export $filename` : export the final audio data of the selected composition to a wav file : NOT IMPLEMENTED

## Component Selection:
* `select -composition $composition` : select the composition to work on : NO COMPOSITION COMMANDS IMPLEMENTED
* `select -c|-s|-k -next|-previous` : select the composition/source/keyframe to work on
* `select -source $SourceName` : selects the source with $sourcename in the selected composition to edit
* `select -keyframe $keyframe_number` : selects the source with $sourcename in the selected composition to edit

## Component Creation:
* `add -source $SourceName $TrackNumber` : add the source with name $SourceName and audio track at index $trackNumber to the current composition
* `add -keyframe $Time_s` : add a keyframe with default properties to the current source
* `add -keyframe $Time_S -properties $radius $theta $phi $scale $cw_rotation` : add keyframe to the current source with given properties: NOT IMPLEMENTED

## Editing Properties:
* `set -c|-s|-k -property $PropertyName $PropertyValue` : sets the selected property to the given value if it exists and $propertyvalue is the right value

## Component Destruction:
* `remove -source $SourceName` : add the source with name $SourceName and audio track at index $trackNumber to the current composition
* `remove -keyframe $keyframe_number` : add a keyframe with default properties to the current source

## Audio Playback:
* `play -c|-s` : starts playback of currently select composition or source, depending on option
* `pause -c|-s` : stops playback of currently select composition or source, depending on option
* `goto -c|-s $Time_S` : sets time used for audio playback

## List Components:
* `list -c|-s|-k` : lists all components
