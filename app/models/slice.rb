class Slice < ActiveRecord::Base
  validates_presence_of :time, :lat, :lon, :avgvel
  validates_numericality_of :avgvel, :greater_than_or_equal_to => 0, :less_than => 100
  validates_numericality_of :lon, :startlon,
    :greater_than_or_equal_to => -180, :less_than_or_equal_to => 180
  validates_numericality_of :lat, :startlat,
    :greater_than_or_equal_to => -90, :less_than_or_equal_to => 90
end
