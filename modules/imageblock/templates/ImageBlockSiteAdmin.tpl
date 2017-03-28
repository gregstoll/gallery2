{*
 * $Revision: 16903 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Image Block Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Show"} </h3>

  <input type="checkbox" id="cbHeading"
   name="{g->formVar var="form[heading]"}" {if $form.heading}checked="checked"{/if}/>
  <label for="cbHeading">
    {g->text text="Heading"}
  </label>
  <br/>

  <input type="checkbox" id="cbTitle"
   name="{g->formVar var="form[title]"}" {if $form.title}checked="checked"{/if}/>
  <label for="cbTitle">
    {g->text text="Title"}
  </label>
  <br/>

  <input type="checkbox" id="cbDate"
   name="{g->formVar var="form[date]"}" {if $form.date}checked="checked"{/if}/>
  <label for="cbDate">
    {g->text text="Date"}
  </label>
  <br/>

  <input type="checkbox" id="cbViews"
   name="{g->formVar var="form[views]"}" {if $form.views}checked="checked"{/if}/>
  <label for="cbViews">
    {g->text text="View Count"}
  </label>
  <br/>

  <input type="checkbox" id="cbOwner"
   name="{g->formVar var="form[owner]"}" {if $form.owner}checked="checked"{/if}/>
  <label for="cbOwner">
    {g->text text="Owner"}
  </label>
  <br/>
</div>

{if isset($ImageBlockSiteAdmin.list)}
<div class="gbBlock">
  <h3> {g->text text="Frames"} </h3>

  <p class="giDescription">
    <a href="{$ImageBlockSiteAdmin.sampleUrl}"> {g->text text="View samples"} </a>
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Album Frame"}
    </td><td>
      <select name="{g->formVar var="form[albumFrame]"}">
	{html_options options=$ImageBlockSiteAdmin.list selected=$form.albumFrame}
      </select>
    </td>
  </tr><tr>
    <td>
      {g->text text="Photo Frame"}
    </td><td>
      <select name="{g->formVar var="form[itemFrame]"}">
	{html_options options=$ImageBlockSiteAdmin.list selected=$form.itemFrame}
      </select>
    </td>
  </tr></table>
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>

<div class="gbBlock">
  <h3> {g->text text="External Image Block"} </h3>

  <p class="giDescription">
    {g->text text="Use a PHP block like the one shown below to include an image block in a PHP page outside of Gallery."}
  </p>
  <p class="giInfo">
    &lt;?php @readfile('{g->url arg1="view=imageblock.External" arg2="blocks=randomImage" arg3="show=title" forceDirect=true forceSessionId=false forceFullUrl=true}'); ?&gt;
  </p>
  <table class="gbDataTable"><tr>
    <td> {$ImageBlockSiteAdmin.prefix}blocks</td>
    <td> {g->text text="Pipe(|) separate list chosen from: randomImage, recentImage, viewedImage, randomAlbum, recentAlbum, viewedAlbum, dailyImage, weeklyImage, monthlyImage, dailyAlbum, weeklyAlbum, monthlyAlbum, specificItem; default is randomImage"} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}show&nbsp;*</td>
    <td> {g->text text="Pipe(|) separated list chosen from: title, date, views, owner, heading, fullSize, rawImage; the value can also be: none"} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}itemId </td>
    <td> {g->text text="Limit the item selection to the subtree of the gallery under the album with the given id; or the id of the item to display when used with specificItem block type"} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}maxSize </td>
    <td> {g->text text="Scale images to this maximum size. If used alone Gallery will locate the most-closely-sized image to the specified value - larger images will be scaled down as necessary in your browser. If specified along with %sshow=fullSize the full size image will always be used and scaled down as necessary." arg1=$ImageBlockSiteAdmin.prefix} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}exactSize </td>
    <td> {g->text text="Just like %smaxSize except that it will not substitute an image smaller than the size you request, so you'll get the closest match in size possible.  Note that this may use a lot more bandwidth if a much larger image has to be scaled down in your browser." arg1=$ImageBlockSiteAdmin.prefix} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}link </td>
    <td> {g->text text="Href for link on image; value of none for no link; default is link to item in the Gallery"} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}linkTarget </td>
    <td> {g->text text="Add a link target (for example, to open links in a new browser window)"} </td>
  {if isset($ImageBlockSiteAdmin.list)}
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}itemFrame&nbsp;*</td>
    <td> {g->text text="Image frame to use around images"} </td>
  </tr><tr>
    <td> {$ImageBlockSiteAdmin.prefix}albumFrame&nbsp;*</td>
    <td> {g->text text="Image frame to use around albums"} </td>
  {/if}
  </tr></table>
  <p class="giDescription">
    {g->text text="If a parameter marked with * is omitted then the site default defined above is used."}
  </p>
  {if isset($ImageBlockSiteAdmin.list)}
  <p class="giDescription">
    {g->text text="Image frames require CSS to be displayed correctly. Include the following in the %s section to support image frames." arg1="&lt;head&gt;"}
  </p>
  <p class="giInfo">
    {capture name="cssUrl"}{g->url arg1="controller=imageblock.ExternalCSS" arg2="frames=wood" forceDirect=true forceSessionId=false forceFullUrl=true useAuthToken=false}{/capture}
    &lt;link rel="stylesheet" href="{$smarty.capture.cssUrl|replace:"&":"&amp;"}"/&gt;
  </p>
  <p class="giDescription">
    {g->text text="Specify the frame id (or pipe separated list of ids) in the frames parameter. Omit the parameter to support the image frames configured above. Frame ids are the directory names in the %s directory." arg1="<tt>modules/imageframe/frames</tt>"}
  </p>
  {/if}
</div>
