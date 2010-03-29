require 'test_helper'

class SlicesControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:slices)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create slice" do
    assert_difference('Slice.count') do
      post :create, :slice => { }
    end

    assert_redirected_to slice_path(assigns(:slice))
  end

  test "should show slice" do
    get :show, :id => slices(:one).id
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => slices(:one).id
    assert_response :success
  end

  test "should update slice" do
    put :update, :id => slices(:one).id, :slice => { }
    assert_redirected_to slice_path(assigns(:slice))
  end

  test "should destroy slice" do
    assert_difference('Slice.count', -1) do
      delete :destroy, :id => slices(:one).id
    end

    assert_redirected_to slices_path
  end
end
