class AddcolumnSlices < ActiveRecord::Migration
  def self.up
    add_column :slices, :duration, :float
  end

  def self.down
    remove_column :slices, :duration
  end
end
