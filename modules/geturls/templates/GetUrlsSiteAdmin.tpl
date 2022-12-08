{*
 * $Revision: 996 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Formatted URLs Admin"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Formatted URL Options"} </h3>
  <p class="gbDescription"> 
    {g->text text="You may choose which formatted URL options you would like to have available to your users."}
  </p>

  <input type="checkbox" id="cbHtmlLink"{if $form.HtmlLink} checked="checked"{/if}
   name="{g->formVar var="form[HtmlLink]"}"/>
  <label for="cbHtmlLink">
    {g->text text="HTML link to page with image"}
  </label>
  <br/>
  
  <input type="checkbox" id="cbHtmlInline"{if $form.HtmlInline} checked="checked"{/if}
   name="{g->formVar var="form[HtmlInline]"}"/>
  <label for="cbHtmlInline">
    {g->text text="HTML inline image"}
  </label>
  <br/>

  <input type="checkbox" id="cbHtmlThumbnail"{if $form.HtmlThumbnail} checked="checked"{/if}
   name="{g->formVar var="form[HtmlThumbnail]"}"/>
  <label for="cbHtmlThumbnail">
    {g->text text="HTML clickable thumbnail"}
  </label>
  <br/>

  <input type="checkbox" id="cbHtmlResize2Full"{if $form.HtmlResize2Full} checked="checked"{/if}
   name="{g->formVar var="form[HtmlResize2Full]"}"/>
  <label for="cbHtmlResize2Full">
    {g->text text="HTML clickable resized to full"}
  </label>
  <br/>

  <input type="checkbox" id="cbBbLink"
   name="{g->formVar var="form[BbLink]"}" {if $form.BbLink}checked="checked"{/if}/>
  <label for="cbBbLink">
    {g->text text="BBCode link to page with image"}
  </label>
  <br/>
  
  <input type="checkbox" id="cbBbInline"{if $form.BbInline} checked="checked"{/if}
   name="{g->formVar var="form[BbInline]"}"/>
  <label for="cbBbInline">
    {g->text text="BBCode inline image"}
  </label>
  <br/>

  <input type="checkbox" id="cbBbThumbnail"{if $form.BbThumbnail} checked="checked"{/if}
   name="{g->formVar var="form[BbThumbnail]"}"/>
  <label for="cbBbThumbnail">
    {g->text text="BBCode clickable thumbnail"}
  </label>
  <br/>

  <input type="checkbox" id="cbBbResize2Full"{if $form.BbResize2Full} checked="checked"{/if}
   name="{g->formVar var="form[BbResize2Full]"}"/>
  <label for="cbBbResize2Full">
    {g->text text="BBCode clickable resized to full"}
  </label>
  <br/>

  <h4> {g->text text="Show URLs available for:"} </h4>

  <input type="radio" id="rbGuestMode"{if $form.guestMode} checked="checked"{/if}
   name="{g->formVar var="form[guestMode]"}" value="1"/>
  <label for="rbGuestMode">
    {g->text text="Anonymous users"}
  </label>
  <br/>

  <input type="radio" id="rbUserMode"{if !$form.guestMode} checked="checked"{/if}
   name="{g->formVar var="form[guestMode]"}" value="0"/>
  <label for="rbUserMode">
    {g->text text="Active user"}
  </label>
  <br/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Miscellaneous Data"} </h3>
  <p class="gbDescription">
    {g->text text="If you are running an embedded installation you may find item Id reporting handy."}
  </p>
  <input type="checkbox" id="cbMiscItemId"{if $form.MiscItemId} checked="checked"{/if}
   name="{g->formVar var="form[MiscItemId]"}"/>
  <label for="cbMiscItemId">
    {g->text text="Report item Id"}
  </label>
  <br/>

  <input type="checkbox" id="cbMiscThumbnailId"{if $form.MiscThumbnailId} checked="checked"{/if}
   name="{g->formVar var="form[MiscThumbnailId]"}"/>
  <label for="cbMiscThumbnailId">
    {g->text text="Report thumbnail Id"}
  </label>
  <br/>
  
  <input type="checkbox" id="cbMiscResizeId"{if $form.MiscResizeId} checked="checked"{/if}
   name="{g->formVar var="form[MiscResizeId]"}"/>
  <label for="cbMiscResizeId">
    {g->text text="Report resize Id (if loaded)"}
  </label>
  <br/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Item Summaries"} </h3>
  <input type="checkbox" id="cbShowItemSummaries"{if $form.showItemSummaries} checked="checked"{/if}
   name="{g->formVar var="form[showItemSummaries]"}"/>
  <label for="cbShowItemSummaries">
    {g->text text="Show formatted URLs for each thumbnail in album views"}
  </label>
  <br/>
  <select name="{g->formVar var="form[itemSummariesWidth]"}">
	{html_options values=$GetUrlsAdmin.itemSummariesWidthList
	 selected=$form.itemSummariesWidth output=$GetUrlsAdmin.itemSummariesWidthList}
  </select>
  <label for="cbitemSummariesWidth">
    {g->text text="Width of formatted URL in album views"}
  </label>
  <br/>
</div>
<div class="gbBlock">
  <h3> {g->text text="Show IE Links"} </h3>
  <input type="checkbox" id="cbShowIeLinks"{if $form.showIeLinks} checked="checked"{/if}
   name="{g->formVar var="form[showIeLinks]"}"/>
  <label for="cbShowIeLinks">
    {g->text text="Show links to copy the markup code to the clipboard (Internet Explorer Only)"}
  </label>
  <br/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Warning Handling"} </h3>
  <p class="gbDescription">
    {g->text text="If you try to get the formatted URLs for an image which may not be publicly accessible, we warn you that this may result in broken links. If you wish, you may choose to suppress these warnings."}
  </p>
  <input type="checkbox" id="cbSuppressSource"{if $form.suppressSourceWarning} checked="checked"{/if}
   name="{g->formVar var="form[suppressSourceWarning]"}"/>
  <label for="cbSuppressSource">
    {g->text text="Suppress warnings about images not being publicly viewable"}
  </label>
  <br/>

  <input type="checkbox" id="cbSuppressResized"{if $form.suppressResizedWarning} checked="checked"{/if}
   name="{g->formVar var="form[suppressResizedWarning]"}"/>
  <label for="cbSuppressResized">
    {g->text text="Suppress warnings about images not having full sized versions publicly viewable"}
  </label>
  <br/>

</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
