class SlicesController < ApplicationController
  before_filter :find_slice,
    :only => [:show, :edit, :update, :destroy]

  # GET /slices
  # GET /slices.xml
  def index
    @slices = Slice.find(:all)

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @slices }
    end
  end

  # GET /slices/1
  # GET /slices/1.xml
  def show
    # The before_filter find_slice is run beforehand
    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @slice }
    end
  end

  # GET /slices/new
  # GET /slices/new.xml
  def new
    @slice = Slice.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @slice }
    end
  end

  # GET /slices/1/edit
  def edit
    # The before_filter find_slice is run beforehand
  end

  # POST /slices
  # POST /slices.xml
  def create
    @slice = Slice.new(params[:slice])

    respond_to do |format|
      if @slice.save
        flash[:notice] = 'Slice was successfully created.'
        format.html { redirect_to(@slice) }
        format.xml  { render :xml => @slice, :status => :created, :location => @slice }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @slice.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /slices/1
  # PUT /slices/1.xml
  def update
    # The before_filter find_slice is run beforehand

    respond_to do |format|
      if @slice.update_attributes(params[:slice])
        flash[:notice] = 'Slice was successfully updated.'
        format.html { redirect_to(@slice) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @slice.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /slices/1
  # DELETE /slices/1.xml
  def destroy
    # The before_filter find_slice is run beforehand
    @slice.destroy

    respond_to do |format|
      format.html { redirect_to(slices_url) }
      format.xml  { head :ok }
    end
  end

  private
  def find_slice
    @slice = Slice.find(params[:id])
  end

end
