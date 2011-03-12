require 'slice'

class Congest
  def self.callAlgo(instanceid, lasttime)
    # Build the conditions for the sub-set of slices
    conds = {}
    conds[:instanceid] = instanceid
    conds[:time] = (lasttime - 2.hours)..(lasttime + 1.hours)

    # Query for the slices
    slices = Slice.all(:conditions => conds)

    if slices.empty?
      return 0
    end
    
    # Pass the list of slices on to the algorithm, then return its return value
    Congest.predict1(slices)
  end

  def self.predict1(slices)
    # Now we must implement our actual algorithm here with the list of slices!
    #slices[-1].avgvel
    slices.size
  end
end
