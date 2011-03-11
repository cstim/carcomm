require 'test_helper'
require 'geo'

class SliceTest < ActiveSupport::TestCase
  # Replace this with your real tests.
#  test "the truth" do
#    assert true
#  end
  test "geo haversine_dist" do
    assert Geo.haversine_dist(2,4,3,5)# == 314283.255073684 
  end
end
