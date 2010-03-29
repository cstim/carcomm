class Slice < ActiveRecord::Base
  validates_presence_of :time, :lat, :lon
end
