class SlicesController < ApplicationController
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
    @slice = Slice.find(params[:id])

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
    @slice = Slice.find(params[:id])
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
    @slice = Slice.find(params[:id])

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
    @slice = Slice.find(params[:id])
    @slice.destroy

    respond_to do |format|
      format.html { redirect_to(slices_url) }
      format.xml  { head :ok }
    end
  end
end
