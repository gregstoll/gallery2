{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{*
 * This will display the tabs assuming the feature is enabled.
 *}

<div class="gbTabBar">
  {if ($mode == 'General')}
    <span class="giSelected o"><span>
      {g->text text="General Settings"}
    </span></span>
  {else}
    <span class="o"><span>
      <a accesskey="g" href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapSiteAdmin"}">{g->text text="<u>G</u>eneral Settings"}</a>
    </span></span>
  {/if}
  {if isset($form.GoogleOverviewFeature) and $form.GoogleOverviewFeature}
    {if ($mode == 'GoogleOverview')}
      <span class="giSelected o"><span>
        {g->text text="Google Overview Settings"}
      </span></span>
    {else}
      <span class="o"><span>
        <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapGoogleOverviewAdmin"}">{g->text text="Google Overview Settings"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.GZoomFeature) and $form.GZoomFeature}
    {if ($mode == 'GZoom')}
      <span class="giSelected o"><span>
        {g->text text="GZoom Settings"}
      </span></span>
    {else}
      <span class="o"><span>
        <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapGZoomAdmin"}">{g->text text="GZoom Settings"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.ThemeFeature) and $form.ThemeFeature}
    {if ($mode == 'Theme')}
      <span class="giSelected o"><span>
        {g->text text="Theme Settings"}
      </span></span>
    {else}
      <span class="o"><span>
        <a accesskey="t" href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapThemeAdmin"}">{g->text text="<u>T</u>heme Settings"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.MarkerFeature) and $form.MarkerFeature}
    {if ($mode == 'Markers')}
      <span class="giSelected o"><span>
        {g->text text="Markers Settings"}
      </span></span>
    {else}
      <span class="o"><span>
        <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapMarkersAdmin"}">{g->text text="Markers Settings"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.LegendFeature) and $form.LegendFeature}
    {if ($mode == 'Legend')}
      <span class="giSelected o"><span>
        {g->text text="Legend Management"}
      </span></span>
    {else}
      <span class="o"><span>
        <a accesskey="l" href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapLegendAdmin"}">{g->text text="<u>L</u>egend Management"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.FilterFeature) and $form.FilterFeature}
    {if ($mode == 'Filter')}
      <span class="giSelected o"><span>
        {g->text text="Filter Management"}
      </span></span>
    {else}
      <span class="o"><span>
        <a accesskey="f" href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapFilterAdmin"}">{g->text text="<u>F</u>ilter Management"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.GroupFeature) and $form.GroupFeature}
    {if ($mode == 'Group')}
      <span class="giSelected o"><span>
        {g->text text="Group Management"}
      </span></span>
    {else}
      <span class="o"><span>
        <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapGroupAdmin"}">{g->text text="Group Management"}</a>
      </span></span>
    {/if}
  {/if}
  {if isset($form.RouteFeature) and $form.RouteFeature}
    {if ($mode == 'Routes')}
      <span class="giSelected o"><span>
        {g->text text="Routes Management"}
      </span></span>
    {else}
      <span class="o"><span>
        <a accesskey="r" href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mapv3.MapRouteAdmin"}">{g->text text="<u>R</u>outes Management"}</a>
      </span></span>
    {/if}
  {/if}
</div>