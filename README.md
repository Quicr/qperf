# qperf

Utility to evaluate relay performance.  

## Command Line Programs

### src/qperf_pub

**qperf_pub** is a publisher that publishes based on the `-c <config.ini>` profile.  

### src/qperf_sub

**qperf_sub** is a subscriber that consumes the tracks based on the `-c <config.ini>` profile. 

It is intended that the same `config.ini` file be used for both pub and sub.  This way they have
the same settings.

## config.ini

Profile configuration per track is configured in a `config.ini` file that is provided to the program using
the `-c` option. See the [config.ini template](templates/config.template.ini) for an example configuration. 

Each section in the `config.ini` defines a test for a publish track and subscribe track.
The `namespace` and `name` together should be **unique** for the section, which is the track. 


> [!IMPORTANT]
> Each section **MUST** not share the same `namespace + name`. If namespace is the same, the name
> should be different.
