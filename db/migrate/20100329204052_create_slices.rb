class CreateSlices < ActiveRecord::Migration
  def self.up
    create_table :slices do |t|
      t.datetime :time
      t.float :lat
      t.float :lon
      t.float :avgvel
      t.float :headingdeg
      t.float :dist
      t.float :startlat
      t.float :startlon

      t.timestamps
    end
  end

  def self.down
    drop_table :slices
  end
end
