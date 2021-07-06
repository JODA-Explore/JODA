#!/bin/bash

# Get number of executions
num_runs=$(cat runs.txt)

for queryfile in *.joda; do
  echo "Found query file $queryfile"
  # Get ID
  id="$(basename -- $queryfile .joda)"

  # Prepare directory
  mkdir -p "runs/$id" 

  # Check for config
  if [ -f "$id.conf" ]; then
    echo "Found $id.conf, using custom config"
    # Copy config
    cp "$id.conf" "runs/$id/joda.conf"
  fi

  # Copy query file
  cp "$queryfile" "runs/$id/$queryfile"

  
  cd "runs/$id" || return

  # Dump config
  joda --dump-config &> "config-dump.txt"

  # Execute benchmark
  for i in $(seq 1 $num_runs); do 
    echo "Executing $i/$num_runs: 'joda --noninteractive --logtostderr -c -b --benchfile $id.json --queryfile \"$queryfile\"'"
    joda --noninteractive --logtostderr -c -b --benchfile $id.json --queryfile "$queryfile" &> "log.$i.txt"
  done

  cd "../.." || return
  
done
