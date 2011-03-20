class ChangeLatlonDouble < ActiveRecord::Migration
  def self.up
    change_column :slices, :startlat, :double
    change_column :slices, :startlon, :double
    change_column :slices, :lat, :double
    change_column :slices, :lon, :double
  end

  def self.down
    change_column :slices, :startlat, :float
    change_column :slices, :startlon, :float
    change_column :slices, :lat, :float
    change_column :slices, :lon, :float
  end
end
