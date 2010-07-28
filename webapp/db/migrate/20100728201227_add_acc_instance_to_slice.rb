class AddAccInstanceToSlice < ActiveRecord::Migration
  def self.up
    add_column :slices, :startaccuracy, :float
    add_column :slices, :endaccuracy, :float
    add_column :slices, :instanceid, :integer
    add_column :slices, :categoryid, :integer
  end

  def self.down
    remove_column :slices, :categoryid
    remove_column :slices, :instanceid
    remove_column :slices, :endaccuracy
    remove_column :slices, :startaccuracy
  end
end
